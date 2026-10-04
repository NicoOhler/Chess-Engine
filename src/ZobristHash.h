#pragma once
#include "definitions.h"
#include "MoveGenerator.h"
#include "BitBoard.h"
#include <random>

using namespace BitBoard;

const uint64 RAND_SEED = 42; // for reproducibility

class ZobristHash
{
public:
    ZobristHash() { initialize(); }

    uint64 computeInitialHash(Board &board);
    uint64 updateHash(Board &board, Move move, UndoInfo undo);

private:
    uint64 whites_turn;
    uint64 piece_at_square[64][12];
    uint64 castling_rights[16];
    uint64 en_passant_file[8];

    void initialize();
};
