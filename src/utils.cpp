#include "utils.h"
#include "Evaluation.h"

Piece getPromotionPiece(Promotion promotion, bool white_to_move)
{
    switch (promotion)
    {
    case PROMOTE_TO_QUEEN:
        return white_to_move ? WHITE_QUEEN : BLACK_QUEEN;
    case PROMOTE_TO_ROOK:
        return white_to_move ? WHITE_ROOK : BLACK_ROOK;
    case PROMOTE_TO_BISHOP:
        return white_to_move ? WHITE_BISHOP : BLACK_BISHOP;
    case PROMOTE_TO_KNIGHT:
        return white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT;
    default:
        assert(false, "Invalid promotion piece");
    }
}

std::string getSquareName(Position square)
{
    assert(square >= 0 && square < 64, "Invalid square");
    return char('a' + modPow2(square, 8)) + std::to_string(square / 8 + 1);
}

Position getSquareIndex(std::string square)
{
    assert(square.length() == 2, "Invalid square");
    char col = square[0];
    char row = square[1];
    assert(col >= 'a' && col <= 'h' && row >= '1' && row <= '8', "Invalid square");
    return 8 * (row - '1') + (col - 'a');
}

Score getPieceValue(Piece piece)
{
    switch (piece)
    {
    case WHITE_PAWN:
        return PAWN_VALUE;
    case WHITE_KNIGHT:
        return KNIGHT_VALUE;
    case WHITE_BISHOP:
        return BISHOP_VALUE;
    case WHITE_ROOK:
        return ROOK_VALUE;
    case WHITE_QUEEN:
        return QUEEN_VALUE;
    case WHITE_KING:
        return KING_VALUE;
    case BLACK_PAWN:
        return PAWN_VALUE;
    case BLACK_KNIGHT:
        return KNIGHT_VALUE;
    case BLACK_BISHOP:
        return BISHOP_VALUE;
    case BLACK_ROOK:
        return ROOK_VALUE;
    case BLACK_QUEEN:
        return QUEEN_VALUE;
    case BLACK_KING:
        return KING_VALUE;
    default:
        assert(false, "Invalid piece");
    }
}

uint64 modPow2(uint64 n, uint64 m)
{
    return n & (m - 1); // fast modulo for power of 2
}
