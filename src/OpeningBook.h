#pragma once

#include "Move.h"
#include "ZobristHash.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>

const std::string TESTING_BOOK = "../data/opening_books/Rebel.bin";
const std::string BALANCED_BOOK = "../data/opening_books/book.bin";
const std::string SOTA_BOOK = "../data/opening_books/Performance.bin";

// "official" polyglot book entry structure (big-endian)
struct PolyglotBookEntry
{
    Hash hash;
    Move16 move;   // 16-bit from-to-promotion encoding
    uint16 weight; // weight that indicates move quality (higher weight = better move)
    uint32 learn;  // implementation-specific learning value (not used in this engine)
};

// internal representation of a book entry for easier access to move data
struct BookEntry
{
    Hash hash;
    Move move;
    uint16 weight;

    std::string toString() const;
};

class OpeningBook
{
public:
    OpeningBook();
    ~OpeningBook();

    void initializeBook(const std::string &book_path);
    Move getBookMove(const Hash &hash);

private:
    BookEntry *book;
    uint64 num_entries;
    void freeBook();
    uint64 getBookIndex(const Hash &hash);
};