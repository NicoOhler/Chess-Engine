#include "TranspositionTable.h"

TranspositionTable::TranspositionTable(int megabytes)
{
    uint64 required_size = megabytes * 1024 * 1024 / sizeof(TranspositionEntry);
    table_size = 1;
    while (table_size <= required_size)
        table_size *= 2;
    table = new TranspositionEntry[table_size]();
}

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
    bool is_position_in_table = entry.hash == hash;

    // always store new positions
    if (is_position_in_table)
    {
        // do not store if present entry has greater depth
        if (entry.remaining_depth > remaining_depth)
            return;

        // do not store if present entry is exact, while new is not exact, keep present entry
        if (entry.type == EXACT && type != EXACT)
            return;
    }

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