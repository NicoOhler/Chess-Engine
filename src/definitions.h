#pragma once
#include <string>

typedef unsigned long long uint64;
typedef signed long Score;
typedef uint64 Bitboard;
typedef uint64 Hash;
typedef unsigned char uint8;
typedef signed char int8;
typedef signed char Position;
typedef Position Direction;
typedef unsigned char Piece;
typedef unsigned char Mode;
typedef unsigned char GameState;
typedef signed char Clock;
typedef uint64 Milliseconds;

// basic constants
const Position NUM_SQUARES = 64;
const Position NUM_ROWS = 8;
const Position NUM_COLS = 8;
const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const int MAX_MOVES = 200;
const Score POS_INFINITY = 2147483647;
const Score NEG_INFINITY = -2147483648;
const Clock HALF_MOVE_CLOCK_RESET = -1;
const Clock HALF_MOVE_CLOCK_LIMIT = 100;

// directions
const Position UP = 8;
const Position DOWN = -8;
const Position LEFT = -1;
const Position RIGHT = 1;
const Position UP_LEFT = 7;
const Position UP_RIGHT = 9;
const Position DOWN_LEFT = -9;
const Position DOWN_RIGHT = -7;

// engine
const bool ENABLE_MOVE_SORTING = false;
const Milliseconds SEARCH_TIME_LIMIT = 5000; // milliseconds
const int TRANSPOSITION_TABLE_SIZE = 256;    // megabytes
const int DEFAULT_PERFT_DEPTH = 7;
const int MAX_SEARCH_DEPTH = 4;
const int MAX_QUIESCENCE_DEPTH = 12;

enum PIECES
{
    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING,
    EMPTY,
    UNDO
};

enum PIECE_SYMBOLS
{
    WHITE_PAWN_SYMBOL = 'P',
    WHITE_KNIGHT_SYMBOL = 'N',
    WHITE_BISHOP_SYMBOL = 'B',
    WHITE_ROOK_SYMBOL = 'R',
    WHITE_QUEEN_SYMBOL = 'Q',
    WHITE_KING_SYMBOL = 'K',
    BLACK_PAWN_SYMBOL = 'p',
    BLACK_KNIGHT_SYMBOL = 'n',
    BLACK_BISHOP_SYMBOL = 'b',
    BLACK_ROOK_SYMBOL = 'r',
    BLACK_QUEEN_SYMBOL = 'q',
    BLACK_KING_SYMBOL = 'k',
};

enum ROWS
{
    ROW_1,
    ROW_2,
    ROW_3,
    ROW_4,
    ROW_5,
    ROW_6,
    ROW_7,
    ROW_8
};

enum COLUMNS
{
    COL_A,
    COL_B,
    COL_C,
    COL_D,
    COL_E,
    COL_F,
    COL_G,
    COL_H
};

enum MODES
{
    UCI_MODE = 'u',
    PLAY_MODE = 'c',
    PERFT_MODE = 'p',
    SEARCH_MODE = 's'
};

enum GAME_STATE
{
    IN_PROGRESS,
    CHECKMATE,
    DRAW // equivalent to stalemate
};