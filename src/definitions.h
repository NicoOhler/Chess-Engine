#pragma once
#include <string>
#include <inttypes.h>

typedef unsigned long long uint64;
// typedef signed long Score;
typedef int32_t Score;
typedef uint64 Bitboard;
typedef uint64 Hash;
typedef unsigned char uint8;
typedef signed char int8;
typedef signed char Position;
typedef Position Direction;
typedef unsigned char Promotion;
typedef unsigned char CastlingRights;
typedef unsigned char PromotionAndCastlingRights;
typedef unsigned char Piece;
typedef unsigned char PieceSymbol;
typedef unsigned char Mode;
typedef unsigned char GameState;
typedef signed char Clock;
typedef uint64 Milliseconds;

// basic constants
const Position NUM_SQUARES = 64;
const Position NUM_ROWS = 8;
const Position NUM_COLS = 8;
const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
const int MAX_MOVES = 218;
const Score POS_INFINITY = 2147483647;
const Score NEG_INFINITY = -2147483648;
const Score USED_MOVE = NEG_INFINITY;
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
const Milliseconds SEARCH_TIME_LIMIT = 5000; // milliseconds
const int TRANSPOSITION_TABLE_SIZE = 256;    // megabytes
const int DEFAULT_PERFT_DEPTH = 6;
const int MAX_SEARCH_DEPTH = 12;
const int MAX_QUIESCENCE_DEPTH = 30;

// promotion and castling rights can be stored within a single byte
enum PROMOTION_AND_CASTLING_RIGHTS
{
    NO_PROMOTION_OR_CASTLING = 0,
    PROMOTE_TO_QUEEN = 1 << 0,
    PROMOTE_TO_ROOK = 1 << 1,
    PROMOTE_TO_BISHOP = 1 << 2,
    PROMOTE_TO_KNIGHT = 1 << 3,
    WHITE_KING_CASTLING = 1 << 4,
    WHITE_QUEEN_CASTLING = 1 << 5,
    BLACK_KING_CASTLING = 1 << 6,
    BLACK_QUEEN_CASTLING = 1 << 7,
    WHITE_CASTLING = WHITE_KING_CASTLING | WHITE_QUEEN_CASTLING,
    BLACK_CASTLING = BLACK_KING_CASTLING | BLACK_QUEEN_CASTLING,
    ALL_CASTLING = WHITE_CASTLING | BLACK_CASTLING
};

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
    PLAYER_VS_PLAYER_MODE = 'l',
    PLAYER_VS_ENGINE_MODE = 'r',
    ENGINE_VS_ENGINE_MODE = 'e',
    PERFT_MODE = 'p',
    BENCHMARK_MODE = 'b',
    SEARCH_MODE = 's'
};

enum GAME_STATE
{
    IN_PROGRESS,
    CHECKMATE,
    DRAW // equivalent to stalemate
};