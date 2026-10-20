#pragma once
#include "definitions.h"
#include "Bitboard.h"

struct Board // represents game state
{
    Board() : white_to_move(true),
              pieces{
                  WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK,
                  WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,
                  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                  BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,
                  BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK},
              piece_bitboards{BLACK_PAWNS_START, WHITE_PAWNS_START, BLACK_KNIGHTS_START, WHITE_KNIGHTS_START, BLACK_BISHOPS_START, WHITE_BISHOPS_START,
                              BLACK_ROOKS_START, WHITE_ROOKS_START, BLACK_QUEENS_START, WHITE_QUEENS_START, BLACK_KING_START, WHITE_KING_START},
              colors{BLACK_PIECES, WHITE_PIECES}, en_passant(NO_EN_PASSANT),
              castling_rights(ALL_CASTLING), occupied(WHITE_PIECES | BLACK_PIECES), half_move_clock(0) {}
    Board(Piece empty) : white_to_move(true),
                         pieces{
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                             EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY},
                         piece_bitboards{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                         colors{0, 0}, en_passant(NO_EN_PASSANT),
                         castling_rights(0), occupied(0), half_move_clock(0) {}
    bool white_to_move;
    Piece pieces[NUM_SQUARES];
    Bitboard piece_bitboards[NUM_PIECES];
    Bitboard colors[NUM_COLORS];

    Bitboard occupied;
    Position en_passant;
    CastlingRights castling_rights;
    HalfMoveClock half_move_clock;
    Hash hash;

    bool isSquareOccupied(Position position) const;

    // static methods
    static Board generateBoardFromFEN(const std::string &fen);
    static void printGameState(const Board &board);
    static void printBoard(const Board &board);
    static void printCastlingRights(const Board &board);
};
