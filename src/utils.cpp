#include "utils.h"
#include "Evaluation.h"

std::string getSquareName(Position square)
{
    assert(square >= 0 && square < 64, "Invalid square");
    return char('a' + square % 8) + std::to_string(square / 8 + 1);
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
    switch (toupper(piece))
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
    default:
        return 0;
    }
}
