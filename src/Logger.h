#pragma once
#include "definitions.h"
#include "iostream"
#include <fstream>

typedef int LogType;

const std::string LOG_FILE = "log.txt"; // does not write to file if empty string
const bool LOG_TO_CONSOLE = true;
const bool APPEND_TO_FILE = false;

const LogType LOG_TYPE_ENABLED = 0x1;
const LogType LOG_TYPE_DISABLED = 0x0;
const LogType WHITE = 0x2;

const LogType CHESS_BOARD = WHITE | LOG_TYPE_ENABLED;
const LogType MOVE_GENERATOR = WHITE;
const LogType PAWN_MOVE = WHITE;
const LogType KNIGHT_MOVE = WHITE;
const LogType BISHOP_MOVE = WHITE;
const LogType ROOK_MOVE = WHITE;
const LogType QUEEN_MOVE = WHITE;
const LogType KING_MOVE = WHITE;
const LogType REMOVE_ILLEGAL_MOVES = WHITE;
const LogType REMAINING_MOVES = WHITE;
const LogType PERFT = WHITE | LOG_TYPE_ENABLED;
const LogType SEARCH = WHITE | LOG_TYPE_ENABLED;
const LogType SEARCH_DEPTHS = WHITE | LOG_TYPE_ENABLED;
const LogType FEN = WHITE | LOG_TYPE_ENABLED;
const LogType TIME = WHITE | LOG_TYPE_ENABLED;
const LogType EVALUATION = WHITE;
const LogType BENCHMARK = WHITE | LOG_TYPE_ENABLED;
const LogType TRANSPOSITION_TABLE_MATCH = WHITE;
const LogType ENGINE_SETTINGS = WHITE | LOG_TYPE_ENABLED;
const LogType UI = WHITE | LOG_TYPE_ENABLED;

void log(LogType logType, std::string message);

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
    void log(LogType logType, std::string message);
    ~Logger();
};
