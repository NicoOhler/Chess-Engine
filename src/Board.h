#pragma once
#include "definitions.h"
#include "Bitboard.h"

struct Board // represents game state
{
    Board() : white_to_move(true),
              white_pawns(WHITE_PAWNS_START), white_rooks(WHITE_ROOKS_START), white_knights(WHITE_KNIGHTS_START),
              white_bishops(WHITE_BISHOPS_START), white_queens(WHITE_QUEENS_START), white_king(WHITE_KING_START),
              black_pawns(BLACK_PAWNS_START), black_rooks(BLACK_ROOKS_START), black_knights(BLACK_KNIGHTS_START),
              black_bishops(BLACK_BISHOPS_START), black_queens(BLACK_QUEENS_START), black_king(BLACK_KING_START),
              white_pieces(WHITE_PIECES), black_pieces(BLACK_PIECES), en_passant(NO_EN_PASSANT),
              castling_rights(ALL_CASTLING), occupied(WHITE_PIECES | BLACK_PIECES), half_move_clock(0) {}
    Board(Piece empty) : white_to_move(true),
                         white_pawns(0), white_rooks(0), white_knights(0),
                         white_bishops(0), white_queens(0), white_king(0),
                         black_pawns(0), black_rooks(0), black_knights(0),
                         black_bishops(0), black_queens(0), black_king(0),
                         white_pieces(0), black_pieces(0), en_passant(NO_EN_PASSANT),
                         castling_rights(0), occupied(0), half_move_clock(0) {}
    // ? maybe merge white_to_move and castling_rights into one bitboard
    bool white_to_move;
    Bitboard white_pawns;
    Bitboard white_rooks;
    Bitboard white_knights;
    Bitboard white_bishops;
    Bitboard white_queens;
    Bitboard white_king;

    Bitboard black_pawns;
    Bitboard black_rooks;
    Bitboard black_knights;
    Bitboard black_bishops;
    Bitboard black_queens;
    Bitboard black_king;

    Bitboard occupied;
    Bitboard white_pieces;
    Bitboard black_pieces;
    Position en_passant;
    CastlingRights castling_rights;
    HalfMoveClock half_move_clock;
    Hash hash;

    Bitboard *getBitboardByPiece(Piece piece);
    Bitboard *getBitboardByPieceSymbol(PieceSymbol piece);
    Piece getPieceAt(Position position) const;
    bool isSquareOccupied(Position position) const;

    // static methods
    static Board generateBoardFromFEN(const std::string &fen);
    static void printGameState(const Board &board);
    static void printBoard(const Board &board);
    static void printCastlingRights(const Board &board);
    static void placePiecesOnBoard(const Board &board, char board_to_print[8][8]);
};
