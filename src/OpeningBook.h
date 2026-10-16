#pragma once

#include "Move.h"
#include "ZobristHash.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

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

    bool initializeBook(const std::string &book_path);
    Move getBookMoveIfAvailable(const Hash &hash);
    bool enabled = false;

private:
    BookEntry *book = nullptr;
    uint64 num_entries = 0;
    void freeBook();
    uint64 getBookIndex(const Hash &hash);
};