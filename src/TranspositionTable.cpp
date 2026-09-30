#include "TranspositionTable.h"

TranspositionTable::TranspositionTable(int megabytes)
{
    table_size = megabytes * 1024 * 1024 / sizeof(TranspositionEntry);
    table = new TranspositionEntry[table_size]();
}

// todo handle indexing and collisions
TranspositionEntry *TranspositionTable::probe(Hash hash)
{
    TranspositionEntry &entry = table[hash % table_size];
    if (entry.hash != hash)
        return nullptr;
    return &entry;
}

void TranspositionTable::store(Hash hash, Score score, Move best_move, int remaining_depth, BoundType type)
{
    // ? current replacement strategy: always overwrite
    TranspositionEntry &entry = table[hash % table_size];
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