#pragma once
#include "Engine.h"
#include <stack>

class UserInterface
{
public:
    void start();
    void parseParameters(int argc, char *argv[]);

private:
    Engine engine;
    Timer timer;
    std::stack<Move> move_history;

    // default parameters
    Mode mode = UCI_MODE;
    std::string fen = START_FEN;
    int ply = -1;
    bool divide = false;
    bool play_vs_ai = true;
    uint64 expected_perft = 0;
    Milliseconds search_time = SEARCH_TIME_LIMIT;

    void startRegularPlay();
    void startSelfPlay();
    void startPerft();
    void startUCI();
    void startSearch();
    void applyAndTrackMove(Move move);
    void printHelp(std::string executable_name);
    Move promptForLegalMove(MoveList legal_moves);
    bool promptForPlayerColor();
    Piece promptForPromotionChoice();
};