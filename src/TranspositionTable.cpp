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
    {
        cache_misses++;
        return nullptr;
    }
    cache_hits++;
    return &entry;
}

void TranspositionTable::store(Hash hash, Score score, Move best_move, int remaining_depth, BoundType type)
{
    Hash index = modPow2(hash, table_size);
    TranspositionEntry &entry = table[index];

    // do not store if present entry has greater depth
    if (entry.remaining_depth > remaining_depth)
        return;

    // do not store if present entry is exact, while new is not exact, keep present entry
    if (entry.hash == hash && (entry.type == EXACT && type != EXACT))
        return;

    entry.hash = hash;
    entry.score = score;
    entry.best_move = best_move;
    entry.remaining_depth = remaining_depth;
    entry.type = type;
}

BoundType TranspositionTable::getBoundType(Score score, Score lower_bound, Score upper_bound)
{
    BoundType type = EXACT;
    if (score <= lower_bound)
        type = UPPER_BOUND;
    else if (score >= upper_bound)
        type = LOWER_BOUND;
    return type;
}

void TranspositionTable::clear()
{
    cache_hits = 0;
    cache_misses = 0;
    for (uint64 i = 0; i < table_size; i++)
    {
        table[i].hash = 0;
        table[i].score = 0;
        table[i].best_move = NULL_MOVE;
        table[i].remaining_depth = 0;
        table[i].type = EXACT;
    }
}

void TranspositionTable::printHitRate()
{
    uint64 total_accesses = cache_hits + cache_misses;
    if (total_accesses == 0)
    {
        log(TRANSPOSITION_TABLE, "Transposition Table Hit Rate: No accesses yet.");
        return;
    }
    double hit_rate = (double)cache_hits / total_accesses * 100.0;
    log(TRANSPOSITION_TABLE, "Transposition Table Hits: " + std::to_string(cache_hits) + ", Misses: " + std::to_string(cache_misses) + ", Accesses: " + std::to_string(total_accesses) + ", Hit Rate: " + std::to_string(hit_rate) + "%");
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