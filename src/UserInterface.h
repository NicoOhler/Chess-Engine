#pragma once
#include "Engine.h"
#include "Timer.h"
#include "UCI.h"
#include <stack>

std::string AVAILABLE_MODES = "u (UCI), b (benchmark), s (search), l (player vs player), r (player vs engine), e (engine vs engine), p (perft)";
enum MODES
{
    UCI_MODE = 'u',
    PLAYER_VS_PLAYER_MODE = 'l',
    PLAYER_VS_ENGINE_MODE = 'r',
    ENGINE_VS_ENGINE_MODE = 'e',
    PERFT_MODE = 'p',
    BENCHMARK_MODE = 'b',
    SEARCH_MODE = 's'
};

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
    uint64 expected_perft = 0;

    void startPlayerVsPlayer();
    void startPlayerVsEngine();
    void startEngineVsEngine();
    void startPerft();
    void startUCI();
    void startSearch();
    void startBenchmark();
    void applyAndTrackMove(Move move, bool play_vs_engine = false);
    void printHelp(std::string executable_name);
    void printGameState();
    Move promptForLegalMove(MoveList legal_moves);
    bool promptForPlayerColor();
    Piece promptForPromotionChoice();
};