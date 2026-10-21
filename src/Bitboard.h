#pragma once
#include "definitions.h"
#include "utils.h"
#include <iostream>
#include <vector>

// Bitboard represents a chess board (64 bit number) with pieces represented as bits
const Bitboard WHITE_PAWNS_START = 0x000000000000FF00;
const Bitboard WHITE_KNIGHTS_START = 0x0000000000000042;
const Bitboard WHITE_BISHOPS_START = 0x0000000000000024;
const Bitboard WHITE_ROOKS_START = 0x0000000000000081;
const Bitboard WHITE_QUEENS_START = 0x0000000000000008;
const Bitboard WHITE_KING_START = 0x0000000000000010;

const Bitboard BLACK_PAWNS_START = 0x00FF000000000000;
const Bitboard BLACK_KNIGHTS_START = 0x4200000000000000;
const Bitboard BLACK_BISHOPS_START = 0x2400000000000000;
const Bitboard BLACK_ROOKS_START = 0x8100000000000000;
const Bitboard BLACK_QUEENS_START = 0x0800000000000000;
const Bitboard BLACK_KING_START = 0x1000000000000000;

const Bitboard LEFT_COL = 0x0101010101010101;
const Bitboard RIGHT_COL = 0x8080808080808080;
const Bitboard WHITE_DOUBLE_PUSH_ROW = 0x0000000000FF0000;
const Bitboard BLACK_DOUBLE_PUSH_ROW = 0x0000FF0000000000;
const Bitboard PROMOTION_ROWS = 0xFF000000000000FF;
const Bitboard WHITE_PIECES = 0x000000000000FFFF;
const Bitboard BLACK_PIECES = 0xFFFF000000000000;
const Position NO_EN_PASSANT = -100;

// castling
const Bitboard WHITE_KING_CASTLING_FREE_SQUARES = 0x0000000000000060;
const Bitboard WHITE_QUEEN_CASTLING_FREE_SQUARES = 0x000000000000000E;
const Bitboard WHITE_QUEEN_CASTLING_SAFE_SQUARES = 0x000000000000000C;
const Bitboard BLACK_KING_CASTLING_FREE_SQUARES = 0x6000000000000000;
const Bitboard BLACK_QUEEN_CASTLING_FREE_SQUARES = 0x0E00000000000000;
const Bitboard BLACK_QUEEN_CASTLING_SAFE_SQUARES = 0x0C00000000000000;

// castling positions
const Position WHITE_KING_START_POSITION = 4;
const Position BLACK_KING_START_POSITION = 60;
const Bitboard WHITE_QUEEN_ROOK_START_POSITION = 0x0000000000000001;
const Bitboard WHITE_KING_ROOK_START_POSITION = 0x0000000000000080;
const Bitboard BLACK_KING_ROOK_START_POSITION = 0x8000000000000000;
const Bitboard BLACK_QUEEN_ROOK_START_POSITION = 0x0100000000000000;

const Position WHITE_QUEEN_ROOK_START_POSITION_NEW = 0;
const Position BLACK_QUEEN_ROOK_START_POSITION_NEW = 56;
const Position WHITE_KING_ROOK_START_POSITION_NEW = 7;
const Position BLACK_KING_ROOK_START_POSITION_NEW = 63;

// Bitboard operations
void movePiece(Bitboard &board, Position from, Position to);
void set(Bitboard &board, Position position);
void clear(Bitboard &board, Position position);
void toggle(Bitboard &board, Position position);
bool isSet(const Bitboard &board, Position position);
std::vector<bool> getBits(const Bitboard &board);
int8 countSetBits(const Bitboard &board);
Position clearRightmostSetBit(Bitboard &board); // sets the rightmost set bit to 0 and returns its index
Position getRightmostSetBit(const Bitboard &board);
void printBitboard(const Bitboard &board, const std::string &title = "");
