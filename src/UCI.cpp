#include "UCI.h"

void UCI::start()
{
    engine_worker_thread = std::thread(&UCI::workerLoop, this);
    std::string command;
    while (true)
    {
        std::getline(std::cin, command);
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
            handlePosition(command.substr(8));
        else if (command.substr(0, 2) == "go")
            handleGo(command.substr(2));
        else
            printInfo("Unknown command: '" + command + "'. Type help for more information.");
    }
}

void UCI::workerLoop()
{
    while (!quit_requested)
    {
        // wait for a search request or quit request
        std::unique_lock<std::mutex> lock(event_queue_mutex);
        event_cv.wait(lock, [this]
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
    engine.stop();
    ScopeLock lock(engine_mutex);
    engine.reset();
}

void UCI::handlePosition(std::string parameters)
{
    // wait for engine to fully stop before changing the board state
    engine.stop();
    ScopeLock lock(engine_mutex);

    // extract FEN and initialize board
    std::string fen = START_FEN;
    if (startsWith(parameters, " startpos"))
        parameters = parameters.substr(9);
    else if (startsWith(parameters, " fen"))
    {
        parameters = parameters.substr(4);
        int end_of_fen = parameters.find(" moves");
        fen = parameters.substr(1, end_of_fen);
        parameters = parameters.substr(end_of_fen);
    }
    engine.initializeBoard(fen);

    // todo extract moves and apply them
    if (startsWith(parameters, " moves"))
    {
        printInfo("Move parsing not yet implemented in UCI interface.");
        return;
    }

    MoveList moves;
    for (int i = 0; i < moves.size; i++)
        engine.makeMove(moves.moves[i], false);
}

void UCI::handleGo(std::string parameters)
{
    engine.stop();
    ScopeLock lock(engine_mutex);
    // parse parameters for time control, depth, etc. if needed
    if (startsWith(parameters, " depth "))
    {
        try
        {
            engine.setSearchDepth(std::stoi(parameters.substr(6)));
        }
        catch (const std::invalid_argument &)
        {
            printInfo("Invalid depth parameter. Using default depth of " + std::to_string(DEFAULT_SEARCH_DEPTH));
            engine.setSearchDepth(DEFAULT_SEARCH_DEPTH);
        }
    }
    else if (startsWith(parameters, " movetime "))
    {
        try
        {
            engine.setTimeLimit(std::stoi(parameters.substr(9)));
        }
        catch (const std::invalid_argument &)
        {
            printInfo("Invalid movetime parameter. Using default time limit of " + std::to_string(DEFAULT_SEARCH_TIME) + "ms");
            engine.setTimeLimit(DEFAULT_SEARCH_TIME);
        }
    }
    else if (startsWith(parameters, " wtime ") || startsWith(parameters, " btime "))
    {
        // todo implement time control for wtime and btime
        printInfo("Time control parsing not yet implemented in UCI interface.");
    }
    ScopeLock event_lock(event_queue_mutex);
    search_requested = true;
    event_cv.notify_one();
}

void UCI::handleStop()
{
    engine.stop();
}

void UCI::handleQuit()
{
    engine.stop();
    std::unique_lock<std::mutex> event_lock(event_queue_mutex);
    quit_requested = true;
    event_cv.notify_one();
    event_lock.unlock();
    engine_worker_thread.join();
    exit(0);
}

void UCI::printInfo(std::string info)
{
    print("info string " + info);
}
