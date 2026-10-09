#pragma once
#include "MoveGenerator.h"
#include "ZobristHash.h"
#include "TranspositionTable.h"
#include "Evaluation.h"
#include "Timer.h"
#include "definitions.h"
#include <iostream>

using namespace BitBoard;
using namespace std;

class Engine
{
public:
    void initializeStartPosition(std::string fen);
    Move search(int max_depth);
    uint64 perft(int depth, bool divide = false);
    Score evaluateBoard();
    bool makeMoveIfLegal(Move move);
    void makeMove(Move move);
    void unmakeMove(Move move);

    // getter and setter
    GameState getGameState(MoveList moves);
    MoveList getLegalMoves();
    void setTimeLimit(Milliseconds time_limit);
    Board getBoard();

private:
    Board board;
    MoveGenerator move_generator = MoveGenerator::getInstance();
    TranspositionTable transposition_table;
    ZobristHash zobrist;
    Evaluation eval;
    Timer timer;
    UndoHistory undo_history;
    uint64 evaluated_nodes = 0;

    void calculateMoveScores(MoveList &moves, TranspositionEntry *entry);
    Score pv_search(int depth, int remaining_depth, Score alpha, Score beta);
    Score quiescence(Score alpha, Score beta, int max_depth = MAX_QUIESCENCE_DEPTH);
};
