#pragma once
#include "MoveGenerator.h"
#include "ZobristHash.h"
#include "TranspositionTable.h"
#include "Evaluation.h"
#include "TimeManagement.h"
#include "definitions.h"
#include <iostream>

using namespace std;

class Engine
{
public:
    Engine();
    Move search();
    void stopSearch(const std::string &reason = "");
    void reset();
    uint64 perft();
    void setPosition(const std::string &fen);
    Score evaluatePosition();
    void makeMove(Move move, bool store_in_history = true);
    void unmakeMove(Move move);

    // getter and setter
    Board getBoard();
    TimeManagement &getTimeManagement();
    GameState getGameState();
    MoveList getLegalMoves();
    void setSearchDepth(int search_depth);
    bool isGameOver();

private:
    Board board = Board::generateBoardFromFEN(START_FEN);
    MoveGenerator move_generator = MoveGenerator::getInstance();
    TranspositionTable transposition_table;
    ZobristHash zobrist;
    Evaluation eval;
    TimeManagement time_management;
    UndoHistory undo_history;
    uint64 evaluated_nodes;
    int search_depth;
    bool search_stopped;
    bool use_opening_book;

    bool makeMoveIfLegal(Move move);
    void calculateMoveScores(MoveList &moves, TranspositionEntry *entry);
    uint64 perft(int remaining_depth, bool divide = false);
    Score pv_search(int depth, int remaining_depth, Score alpha, Score beta);
    Score quiescence(Score alpha, Score beta, int max_depth = MAX_QUIESCENCE_DEPTH);
    void printSearchInfo(int depth, Score score, Move pv_move);
};
