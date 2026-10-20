#include "utils.h"
#include "Evaluation.h"

Position getRow(Position square)
{
    return square / NUM_COLS;
}

Position getCol(Position square)
{
    return modPow2(square, NUM_COLS);
}

std::string getSquareName(Position square)
{
    if (square < 0 || square >= NUM_SQUARES)
        throw std::invalid_argument("Invalid square");
    return char('a' + getCol(square)) + std::to_string(getRow(square) + 1);
}

Position getSquareIndex(const std::string &square)
{
    if (square.length() != 2)
        throw std::invalid_argument("Invalid square");
    char col = square[0];
    char row = square[1];
    if (col < 'a' || col > 'h' || row < '1' || row > '8')
        throw std::invalid_argument("Invalid square");
    return NUM_COLS * (row - '1') + (col - 'a');
}

uint64 modPow2(uint64 n, uint64 m)
{
    return n & (m - 1); // fast modulo for power of 2
}

void print(const std::string &message, bool newline)
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

bool getBoolFromString(const std::string &str)
{
    if (str == "true" || str == "1")
        return true;
    else if (str == "false" || str == "0")
        return false;
    else
        throw std::invalid_argument("Invalid boolean string: " + str);
}
