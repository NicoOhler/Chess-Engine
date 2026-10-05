#pragma once

#include "definitions.h"
#include "Logger.h"
#include <string>
#include <iostream>

#define assert(condition, message)         \
    if (!(condition))                      \
    {                                      \
        std::cerr << message << std::endl; \
        exit(1);                           \
    }

std::string getSquareName(Position square);
Position getSquareIndex(std::string square);
Score getPieceValue(Piece piece);
uint64 modPow2(uint64 n, uint64 m);