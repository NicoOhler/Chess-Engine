#pragma once
#include "definitions.h"
#include "Logger.h"
#include "MoveGenerator.h"

const Score PAWN_VALUE = 100;
const Score KNIGHT_VALUE = 300;
const Score BISHOP_VALUE = 330;
const Score ROOK_VALUE = 500;
const Score QUEEN_VALUE = 900;
const Score KING_VALUE = 20000;
const Score DELTA_VALUE = 800;
const Score MATE_VALUE = 6000000;
const Score DRAW_VALUE = -200;
const Score ISOLATED_PAWN_VALUE = -50;
const Score DOUBLED_PAWN_VALUE = -50;
const Score BLOCKED_PAWN_VALUE = -50;

class Evaluation
{
private:
    MoveGenerator move_generator = MoveGenerator::getInstance();
    Score evaluateMaterial(Board board);
    Score evaluateMobility(Board board);
    Score evaluatePawnStructure(Board board);

public:
    Score evaluateBoard(Board board);
};