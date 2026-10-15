#include "utils.h"
#include "Evaluation.h"

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
        throw std::invalid_argument("Invalid piece: " + std::to_string(piece));
    }
}

uint64 modPow2(uint64 n, uint64 m)
{
    return n & (m - 1); // fast modulo for power of 2
}

void print(std::string message, bool newline)
{
    std::cout << message;
    if (newline)
        std::cout << std::endl;
}

std::string readLineFromStdin()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}

bool contains(const std::string &str, const std::string &substr)
{
    return str.find(substr) != std::string::npos;
}
