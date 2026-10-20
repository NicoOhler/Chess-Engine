#include "UCI.h"

void UCI::start()
{
    opening_book.enabled = opening_book.initializeBook(BOOK_FILE_OPTION_DEFAULT);
    background_thread = std::thread(&UCI::handleBackgroundRequests, this);
    std::string command;
    while (true)
    {
        command = readLineFromStdin();
        if (command == UCI_COMMAND)
            handleUCI();
        else if (command == ISREADY_COMMAND)
            handleIsReady();
        else if (command == UCINEWGAME_COMMAND)
            handleUCINewGame();
        else if (command == STOP_COMMAND)
            handleStop();
        else if (command == QUIT_COMMAND)
            handleQuit();
        else if (command.substr(0, POSITION_COMMAND.length()) == POSITION_COMMAND)
            handlePosition(command);
        else if (command.substr(0, GO_COMMAND.length()) == GO_COMMAND)
            handleGo(command);
        else if (command.substr(0, SETOPTION_COMMAND.length()) == SETOPTION_COMMAND)
            handleSetOption(command);
        else if (command == HELP_COMMAND)
            handleHelp();
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
            std::lock_guard<std::mutex> engine_lock(engine_mutex);
            Move best_move = engine.search();
            print("bestmove " + best_move.toString());
        }
    }
}

void UCI::handleUCI()
{
    print("id name Chess Engine");
    print("id author Nico Ohler");
    print("option name " + USE_OPENING_BOOK_OPTION + " type check default " + USE_OPENING_BOOK_OPTION_DEFAULT);
    print("option name " + BOOK_FILE_OPTION + " type string default " + BOOK_FILE_OPTION_DEFAULT);
    print("uciok");
}

void UCI::handleIsReady()
{
    std::lock_guard<std::mutex> lock(engine_mutex);
    print("readyok");
}

void UCI::handleUCINewGame()
{
    engine.stopSearch();
    std::lock_guard<std::mutex> lock(engine_mutex);
    engine.reset();
}

void UCI::handlePosition(const std::string &command)
{
    // wait for engine to fully stop before changing the board state
    engine.stopSearch();
    std::lock_guard<std::mutex> lock(engine_mutex);

    // extract FEN and initialize board
    std::string fen = START_FEN;
    // no need to check for "startpos" since it is the default FEN
    if (contains(command, " fen"))
        fen = extractArgumentFromCommand(command, " fen ", " moves");
    engine.setPosition(fen);

    if (!contains(command, " moves "))
        return;
    try
    {
        std::string moves = extractArgumentFromCommand(command, " moves ", "");
        char *move_str = strtok(const_cast<char *>(moves.c_str()), " ");
        while (move_str)
        {
            MoveList legal_moves = engine.getLegalMoves();
            bool move_is_legal = false;
            Move move = Move(move_str);
            for (int i = 0; i < legal_moves.size; i++)
                if (legal_moves.moves[i] == move)
                {
                    move_is_legal = true;
                    engine.makeMove(move, false);
                    move_str = strtok(nullptr, " ");
                    break;
                }
            if (!move_is_legal)
                throw std::invalid_argument("Illegal move: " + move.toString());
        }
    }
    catch (const std::exception &e)
    {
        print("Error parsing and applying moves: " + std::string(e.what()));
        engine.setPosition(fen);
    }
}

void UCI::handleGo(const std::string &command)
{
    engine.stopSearch();

    // todo go perft

    // provide a book move if available
    if (opening_book.enabled)
    {
        Move book_move = opening_book.getBookMoveIfAvailable(engine.getBoard().hash);
        if (book_move != NULL_MOVE)
        {
            print("bestmove " + book_move.toString());
            return;
        }
    }

    std::lock_guard<std::mutex> lock(engine_mutex);
    // configure time management based on UCI command parameters
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
    std::lock_guard<std::mutex> background_request_lock(background_request_mutex);
    search_requested = true;
    background_request_cv.notify_one();
}

void UCI::handleSetOption(const std::string &command)
{
    try
    {
        std::string option_name = extractArgumentFromCommand(command, "name ", " value ");
        std::string option_value = extractArgumentFromCommand(command, "value ");
        if (option_name == USE_OPENING_BOOK_OPTION)
            opening_book.enabled = getBoolFromString(option_value);
        else if (option_name == BOOK_FILE_OPTION)
            opening_book.initializeBook(option_value);
        else
            print("No such option");
    }
    catch (const std::exception &e)
    {
        print("Error parsing 'setoption' command parameters: " + std::string(e.what()));
    }
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

void UCI::handleHelp()
{
    print(UCI_HELP);
}

std::string UCI::extractArgumentFromCommand(const std::string &command, const std::string &prefix, const std::string &suffix)
{
    size_t start = command.find(prefix);
    if (start == std::string::npos)
        return "";
    start += prefix.length();
    if (suffix.empty())
        return command.substr(start);
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