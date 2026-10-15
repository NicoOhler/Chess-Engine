#include "UCI.h"

void UCI::start()
{
    background_thread = std::thread(&UCI::handleBackgroundRequests, this);
    std::string command;
    while (true)
    {
        command = readLineFromStdin();
        if (command == "uci")
            handleUCI();
        else if (command == "isready")
            handleIsReady();
        else if (command == "ucinewgame")
            handleUCINewGame();
        else if (command == "stop")
            handleStop();
        else if (command == "quit")
            handleQuit();
        else if (command.substr(0, 8) == "position")
            handlePosition(command);
        else if (command.substr(0, 2) == "go")
            handleGo(command);
        else
            print("Unknown command: '" + command + "'. Type help for more information.");
    }
}

// worker thread loop that handles search and quit requests
void UCI::handleBackgroundRequests()
{
    while (!quit_requested)
    {
        // wait for a search request or quit request
        std::unique_lock<std::mutex> lock(background_request_mutex);
        background_request_cv.wait(lock, [this]
                                   { return search_requested.load() || quit_requested.load(); });

        // handle quit or search request
        if (quit_requested)
            break;

        if (search_requested)
        {
            search_requested = false;
            lock.unlock();
            ScopeLock engine_lock(engine_mutex);
            Move best_move = engine.search();
            print("bestmove " + best_move.toString());
        }
    }
}

void UCI::handleUCI()
{
    print("id name Chess Engine");
    print("id author Nico Ohler");
    print("uciok");
}

void UCI::handleIsReady()
{
    ScopeLock lock(engine_mutex);
    print("readyok");
}

void UCI::handleUCINewGame()
{
    engine.stopSearch();
    ScopeLock lock(engine_mutex);
    engine.reset();
}

void UCI::handlePosition(std::string command)
{
    // wait for engine to fully stop before changing the board state
    engine.stopSearch();
    ScopeLock lock(engine_mutex);

    // extract FEN and initialize board
    std::string fen = START_FEN;
    // no need to check for "startpos" since it is the default FEN
    if (contains(command, " fen"))
        fen = extractArgumentFromCommand(command, " fen ", " moves");
    engine.setPosition(fen);

    // todo extract moves and apply them
    if (contains(command, " moves"))
    {
        print("Move parsing not yet implemented in UCI interface.");
        return;
    }

    MoveList moves;
    for (int i = 0; i < moves.size; i++)
        engine.makeMove(moves.moves[i], false);
}

void UCI::handleGo(std::string command)
{
    engine.stopSearch();
    ScopeLock lock(engine_mutex);
    TimeManagement &time_management = engine.getTimeManagement();
    time_management.reset();
    try
    {
        // parse parameters for time control, depth, etc. if needed
        if (contains(command, " depth "))
            engine.setSearchDepth(std::stoi(extractArgumentFromCommand(command, " depth ")));
        else if (contains(command, " movetime "))
            time_management.configure(extractMillisecondsFromString(extractArgumentFromCommand(command, " movetime ")));
        else if (contains(command, "time"))
        {
            Milliseconds white_time = extractMillisecondsFromString(extractArgumentFromCommand(command, " wtime "));
            Milliseconds black_time = extractMillisecondsFromString(extractArgumentFromCommand(command, " btime "));
            Milliseconds white_increment = extractMillisecondsFromString(extractArgumentFromCommand(command, " winc "));
            Milliseconds black_increment = extractMillisecondsFromString(extractArgumentFromCommand(command, " binc "));
            time_management.configure(white_time, black_time, white_increment, black_increment);
        }
    }
    catch (const std::exception &e)
    {
        print("Error parsing 'go' command parameters: " + std::string(e.what()));
        print("Using default search parameters.");
    }

    // notify background thread to start search
    ScopeLock background_request_lock(background_request_mutex);
    search_requested = true;
    background_request_cv.notify_one();
}

void UCI::handleStop()
{
    engine.stopSearch();
}

void UCI::handleQuit()
{
    // notify background thread to quit and wait for it to finish
    engine.stopSearch();
    std::unique_lock<std::mutex> background_request_lock(background_request_mutex);
    quit_requested = true;
    background_request_cv.notify_one();
    background_request_lock.unlock();
    background_thread.join();
    exit(0);
}

std::string UCI::extractArgumentFromCommand(const std::string &command, const std::string &prefix, const std::string &suffix)
{
    size_t start = command.find(prefix);
    if (start == std::string::npos)
        return "";
    start += prefix.length();
    size_t end = command.find(suffix, start);
    if (end == std::string::npos)
        return command.substr(start);
    return command.substr(start, end - start);
}

Milliseconds UCI::extractMillisecondsFromString(const std::string &string)
{
    if (string.empty())
        return 0;
    return std::stol(string);
}