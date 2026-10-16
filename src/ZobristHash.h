#pragma once
#include "definitions.h"
#include "MoveGenerator.h"
#include "utils.h"
#include "PolyglotKeys.h"

class ZobristHash
{
public:
    ZobristHash();

    uint64 computeInitialHash(Board &board);
    uint64 updateHash(Board &board, Move move, UndoInfo undo);

private:
    uint64 is_whites_turn;
    uint64 piece_at_square[NUM_SQUARES][NUM_PIECES];
    uint64 castling_rights[NUM_CASTLING_RIGHTS];
    uint64 en_passant_file[NUM_COLS];
};
