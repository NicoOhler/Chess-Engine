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
    int ply = DEFAULT_PERFT_DEPTH;
    bool divide = false;
    uint64 expected_perft = 0;
    Milliseconds search_time = SEARCH_TIME_LIMIT;

    void startPlayerVsPlayer();
    void startPlayerVsEngine();
    void startEngineVsEngine();
    void startPerft();
    void startUCI();
    void startSearch();
    void applyAndTrackMove(Move move, bool play_vs_engine = false);
    void printHelp(std::string executable_name);
    void printGameState();
    Move promptForLegalMove(MoveList legal_moves);
    bool promptForPlayerColor();
    Piece promptForPromotionChoice();
};