#pragma once
#include "Engine.h"
#include "OpeningBook.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

// implemented UCI commands
const std::string UCI_COMMAND = "uci";
const std::string ISREADY_COMMAND = "isready";
const std::string UCINEWGAME_COMMAND = "ucinewgame";
const std::string POSITION_COMMAND = "position";
const std::string GO_COMMAND = "go";
const std::string STOP_COMMAND = "stop";
const std::string QUIT_COMMAND = "quit";
const std::string SETOPTION_COMMAND = "setoption";
const std::string HELP_COMMAND = "help";

// available engine options
const std::string USE_OPENING_BOOK_OPTION = "OwnBook";
const std::string USE_OPENING_BOOK_OPTION_DEFAULT = "true";
const std::string BOOK_FILE_OPTION = "BookFile";
const std::string BOOK_FILE_OPTION_DEFAULT = "../data/opening_books/Rebel.bin";

const std::string UCI_HELP = "Available commands:\n"
                             "uci - initialize UCI mode\n"
                             "isready - check if engine is ready\n"
                             "ucinewgame - start a new game\n"
                             "position [fen <FEN> | startpos] [moves <move1> <move2> ...] - set up the board position\n"
                             "go [wtime <time>] [btime <time>] [winc <increment>] [binc <increment>] [movetime <time>] - start searching for the best move\n"
                             "stop - stop searching and return the best move found so far\n"
                             "quit - exit the engine\n"
                             "setoption name <name> value <value> - set engine options\n"
                             "help - display this help message";

class UCI
{
public:
    UCI(Engine &engine) : engine(engine) {}
    void start();

private:
    Engine &engine;
    OpeningBook opening_book;

    // background thread and synchronization
    std::thread background_thread;
    std::condition_variable background_request_cv;
    std::mutex background_request_mutex;
    std::mutex engine_mutex;
    std::atomic<bool> quit_requested{false};
    std::atomic<bool> search_requested{false};
    void handleBackgroundRequests();

    // UCI command handlers
    void handleUCI();
    void handleIsReady();
    void handleUCINewGame();
    void handlePosition(const std::string &command);
    void handleGo(const std::string &command);
    void handleSetOption(const std::string &command);
    void handleStop();
    void handleHelp();
    void handleQuit();

    std::string extractArgumentFromCommand(const std::string &command, const std::string &prefix, const std::string &suffix = " ");
    Milliseconds extractMillisecondsFromString(const std::string &string);
};