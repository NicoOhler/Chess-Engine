#pragma once

#include "definitions.h"
#include "Logger.h"
#include <string>
#include <iostream>
#include <mutex>

Position getRow(Position square);
Position getCol(Position square);
std::string getSquareName(Position square);
Position getSquareIndex(const std::string &square);
Score getPieceValue(Piece piece);
uint64 modPow2(uint64 n, uint64 m);
void print(const std::string &message, bool newline = true);
std::string readLineFromStdin();
bool contains(const std::string &str, const std::string &substr);
bool getBoolFromString(const std::string &str);

class ScopeLock
{
public:
    ScopeLock(std::mutex &mutex) : mutex(mutex) { mutex.lock(); }
    ~ScopeLock() { mutex.unlock(); }

private:
    std::mutex &mutex;
};