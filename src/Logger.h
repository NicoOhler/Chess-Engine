#pragma once
#include "definitions.h"
#include "iostream"
#include <fstream>

typedef int LogType;

const std::string LOG_FILE = "log.txt"; // does not write to file if empty string
const bool LOG_TO_CONSOLE = true;
const bool APPEND_TO_FILE = false;

const LogType DISABLED = 0x0;
const LogType ENABLED = 0x1;

const LogType CHESS_BOARD = ENABLED;
const LogType MOVE_GENERATOR = DISABLED;
const LogType PAWN_MOVE = DISABLED;
const LogType KNIGHT_MOVE = DISABLED;
const LogType BISHOP_MOVE = DISABLED;
const LogType ROOK_MOVE = DISABLED;
const LogType QUEEN_MOVE = DISABLED;
const LogType KING_MOVE = DISABLED;
const LogType REMOVE_ILLEGAL_MOVES = DISABLED;
const LogType REMAINING_MOVES = DISABLED;
const LogType PERFT = ENABLED;
const LogType SEARCH = ENABLED;
const LogType SEARCH_DEPTHS = ENABLED;
const LogType FEN = DISABLED;
const LogType TIME = ENABLED;
const LogType EVALUATION = DISABLED;
const LogType BENCHMARK = ENABLED;
const LogType TRANSPOSITION_TABLE = ENABLED;
const LogType TRANSPOSITION_TABLE_MATCH = DISABLED;
const LogType ENGINE_SETTINGS = DISABLED;
const LogType OPENING_BOOK = DISABLED;
const LogType OPENING_BOOK_MOVE = DISABLED;
const LogType UI = ENABLED;

void log(LogType logType, const std::string &message);

class Logger
{
private:
    static Logger *instance;
    bool print_to_console;
    std::ofstream file;
    std::string filename;
    Logger();

public:
    Logger(Logger &other) = delete;
    void operator=(const Logger &) = delete;
    static Logger *GetInstance();
    void log(LogType logType, const std::string &message);
    ~Logger();
};
