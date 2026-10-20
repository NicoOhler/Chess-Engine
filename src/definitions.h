#pragma once
#include <string>
#include <cstdint>

// useful data types
typedef uint64_t uint64;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int8_t int8;
typedef uint8_t uint8;

typedef uint16 Move16;
typedef int32 Score;
typedef uint64 Bitboard;
typedef uint64 Hash;
typedef signed char Position;
typedef Position Direction;
typedef unsigned char Promotion;
typedef unsigned char CastlingRights;
typedef unsigned char Piece;
typedef unsigned char PieceSymbol;
typedef unsigned char Mode;
typedef std::string GameState;
typedef signed char HalfMoveClock;
typedef int32 Milliseconds;

// basic constants
const Position NUM_SQUARES = 64;
const int8 NUM_COLORS = 2;
const int8 NUM_PIECES = 12;
const int8 NUM_ROWS = 8;
const int8 NUM_COLS = 8;
const int8 NUM_CASTLING_RIGHTS = 4;
const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const int MAX_MOVES = 218;
// not quite min/max of int32 to avoid overflow when negating
const Score POS_INFINITY = 2147483000;
const Score NEG_INFINITY = -2147483000;
const Score USED_MOVE = NEG_INFINITY;
const HalfMoveClock HALF_MOVE_CLOCK_RESET = -1;
const HalfMoveClock HALF_MOVE_CLOCK_LIMIT = 100;

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
const int DEFAULT_PERFT_DEPTH = 10;
const int DEFAULT_SEARCH_DEPTH = 25;
const int MAX_SEARCH_DEPTH = 40;
const int MAX_QUIESCENCE_DEPTH = 30;
const int TRANSPOSITION_TABLE_SIZE_MB = 256;

// time management
const int CLOCK_UPDATE_INTERVAL_NODES = 10000;
const Milliseconds DEFAULT_SEARCH_TIME = 5000;
const Milliseconds FRACTION_OF_TIME_TO_ALLOCATE = 20;
const Milliseconds SUFFICIENT_TIME_FOR_NEXT_ITERATION_FACTOR = 2; // 15;

// benchmark
const int BENCHMARK_PERFT_DEPTH = 6;
const int BENCHMARK_SEARCH_DEPTH = 9;
const int BENCHMARK_EVALUATION_COUNT = 100000;

// game state
const std::string IN_PROGRESS = "In Progress";
const std::string CHECKMATE = "Checkmate";
const std::string DRAW = "Draw"; // equivalent to stalemate

enum COLORS
{
    BLACK,
    WHITE
};

enum PIECES
{
    BLACK_PAWN,
    WHITE_PAWN,
    BLACK_KNIGHT,
    WHITE_KNIGHT,
    BLACK_BISHOP,
    WHITE_BISHOP,
    BLACK_ROOK,
    WHITE_ROOK,
    BLACK_QUEEN,
    WHITE_QUEEN,
    BLACK_KING,
    WHITE_KING,
    EMPTY
};

enum PROMOTION
{
    NO_PROMOTION = 0,
    UNDO = 1,
    PROMOTE_TO_KNIGHT = BLACK_KNIGHT,
    PROMOTE_TO_BISHOP = BLACK_BISHOP,
    PROMOTE_TO_ROOK = BLACK_ROOK,
    PROMOTE_TO_QUEEN = BLACK_QUEEN,
};

enum CASTLING_RIGHTS
{
    NO_CASTLING = 0,
    WHITE_KING_CASTLING = 1 << 0,
    WHITE_QUEEN_CASTLING = 1 << 1,
    BLACK_KING_CASTLING = 1 << 2,
    BLACK_QUEEN_CASTLING = 1 << 3,
    WHITE_CASTLING = WHITE_KING_CASTLING | WHITE_QUEEN_CASTLING,
    BLACK_CASTLING = BLACK_KING_CASTLING | BLACK_QUEEN_CASTLING,
    ALL_CASTLING = WHITE_CASTLING | BLACK_CASTLING
};

enum PIECE_SYMBOLS
{
    BLACK_PAWN_SYMBOL = 'p',
    WHITE_PAWN_SYMBOL = 'P',
    BLACK_KNIGHT_SYMBOL = 'n',
    WHITE_KNIGHT_SYMBOL = 'N',
    BLACK_BISHOP_SYMBOL = 'b',
    WHITE_BISHOP_SYMBOL = 'B',
    BLACK_ROOK_SYMBOL = 'r',
    WHITE_ROOK_SYMBOL = 'R',
    BLACK_QUEEN_SYMBOL = 'q',
    WHITE_QUEEN_SYMBOL = 'Q',
    BLACK_KING_SYMBOL = 'k',
    WHITE_KING_SYMBOL = 'K',
    EMPTY_SYMBOL = ' '
};
const PieceSymbol PIECE_SYMBOLS[NUM_PIECES + 1] = {
    BLACK_PAWN_SYMBOL,
    WHITE_PAWN_SYMBOL,
    BLACK_KNIGHT_SYMBOL,
    WHITE_KNIGHT_SYMBOL,
    BLACK_BISHOP_SYMBOL,
    WHITE_BISHOP_SYMBOL,
    BLACK_ROOK_SYMBOL,
    WHITE_ROOK_SYMBOL,
    BLACK_QUEEN_SYMBOL,
    WHITE_QUEEN_SYMBOL,
    BLACK_KING_SYMBOL,
    WHITE_KING_SYMBOL,
    EMPTY_SYMBOL};

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
