#include "TranspositionTable.h"

TranspositionTable::TranspositionTable(int megabytes)
{
    uint64 required_size = megabytes * 1024 * 1024 / sizeof(TranspositionEntry);
    table_size = 1;
    while (table_size <= required_size)
        table_size *= 2;
    table = new TranspositionEntry[table_size]();
}

// todo handle indexing and collisions
TranspositionEntry *TranspositionTable::probe(Hash hash)
{
    Hash index = modPow2(hash, table_size);
    TranspositionEntry &entry = table[index];
    if (entry.hash != hash)
        return nullptr;
    return &entry;
}

void TranspositionTable::store(Hash hash, Score score, Move best_move, int remaining_depth, BoundType type)
{
    Hash index = modPow2(hash, table_size);
    TranspositionEntry &entry = table[index];
    // keep entry with greater remaining depth
    if (entry.hash == hash && entry.remaining_depth > remaining_depth)
        // keep exact score over bound score
        if (type != EXACT && entry.type == EXACT)
            return;
    entry.hash = hash;
    entry.score = score;
    entry.best_move = best_move;
    entry.remaining_depth = remaining_depth;
    entry.type = type;
}

BoundType TranspositionTable::determineBoundType(Score score, Score lower_bound, Score upper_bound)
{
    BoundType type = EXACT;
    if (score <= lower_bound)
        type = UPPER_BOUND;
    else if (score >= upper_bound)
        type = LOWER_BOUND;
    return type;
}

Score computeTranspositionScore(Score score, int ply)
{
    if (score >= MIN_MATE_VALUE)
        score += ply;
    else if (score <= -MIN_MATE_VALUE)
        score -= ply;
    return score;
}

Score computeOriginalScore(Score score, int ply)
{
    if (score >= MIN_MATE_VALUE)
        score -= ply;
    else if (score <= -MIN_MATE_VALUE)
        score += ply;
    return score;
}