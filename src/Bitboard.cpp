#include "Bitboard.h"

void movePiece(Bitboard &board, Position from, Position to)
{
    board ^= (1ULL << from | 1ULL << to);
}

void set(Bitboard &board, Position position)
{
    board |= 1ULL << position;
}

void clear(Bitboard &board, Position position)
{
    board ^= 1ULL << position;
}

bool isSet(const Bitboard &board, Position position)
{
    return board & (1ULL << position);
}

std::vector<bool> getBits(const Bitboard &board)
{
    std::vector<bool> bits(NUM_SQUARES);
    for (int i = 0; i < NUM_SQUARES; i++)
        bits[i] = board & (1ULL << i);
    return bits;
}

int8 countSetBits(const Bitboard &board)
{
    return __builtin_popcountll(board);
}
Position clearRightmostSetBit(Bitboard &board)
{
    Position index_of_LSB = __builtin_ctzll(board);
    board &= board - 1;
    return index_of_LSB;
}

Position getRightmostSetBit(const Bitboard &board)
{
    return __builtin_ctzll(board);
}

void printBitboard(const Bitboard &board, std::string title)
{
    std::vector<bool> bits = getBits(board);
    std::cout << title << std::endl;
    for (Position row = NUM_ROWS - 1; row >= 0; row--)
    {
        std::cout << row + 1 << " ";
        for (Position col = 0; col < NUM_COLS; col++)
            std::cout << bits[row * NUM_COLS + col] ? "1" : "0";
        std::cout << std::endl;
    }
    std::cout << "  ";
    for (Position col = 0; col < NUM_COLS; col++)
        std::cout << (char)('a' + col);
    std::cout << std::endl
              << std::endl;
}