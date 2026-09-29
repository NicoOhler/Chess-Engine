#pragma once
#include "definitions.h"
#include "Evaluation.h"
#include "MoveGenerator.h"
#include "ZobristHash.h"
#include "BitBoard.h"

using namespace BitBoard;

typedef enum
{
    EXACT,
    LOWER_BOUND,
    UPPER_BOUND
} BoundType;

struct TranspositionEntry
{
    Hash hash = 0;
    Score score = 0;
    Move best_move = NULL_MOVE;
    int remaining_depth = 0;
    BoundType type = EXACT;
    // ? alpha beta

    TranspositionEntry() = default;
};

Score computeTranspositionScore(Score score, int ply);
Score computeOriginalScore(Score score, int ply);

class TranspositionTable
{
public:
    TranspositionTable(int megabytes = TRANSPOSITION_TABLE_SIZE);
    TranspositionEntry *probe(Hash hash);
    void store(Hash hash, Score score, Move best_move, int remaining_depth, BoundType type);

private:
    uint64 table_size;
    TranspositionEntry *table;
};
