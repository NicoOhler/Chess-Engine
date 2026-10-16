#include "UserInterface.h"

void UserInterface::start()
{
    switch (mode)
    {
    case UCI_MODE:
        startUCI();
        break;
    case PLAYER_VS_PLAYER_MODE:
        startPlayerVsPlayer();
        break;
    case PLAYER_VS_ENGINE_MODE:
        startPlayerVsEngine();
        break;
    case ENGINE_VS_ENGINE_MODE:
        startEngineVsEngine();
        break;
    case PERFT_MODE:
        startPerft();
        break;
    case BENCHMARK_MODE:
        startBenchmark();
        break;
    case SEARCH_MODE:
        startSearch();
        break;
    default:
        print("Invalid mode. Choose from the following: " + AVAILABLE_MODES);
        exit(1);
        break;
    }
}

void UserInterface::startBenchmark()
{
    log(BENCHMARK, "Starting benchmark of move generation, evaluation and search.");
    log(BENCHMARK, "Generate all legal moves recursively to a depth of " + std::to_string(BENCHMARK_PERFT_DEPTH) + ".");
    engine.setSearchDepth(BENCHMARK_PERFT_DEPTH);
    timer.start();
    uint64 nodes = engine.perft();
    timer.stop(BENCHMARK);
    log(BENCHMARK, "Nodes searched: " + std::to_string(nodes));

    log(BENCHMARK, "Search for the best move with a depth of " + std::to_string(BENCHMARK_SEARCH_DEPTH) + ".");
    engine.setSearchDepth(BENCHMARK_SEARCH_DEPTH);
    timer.start();
    engine.search();
    timer.stop(BENCHMARK);

    log(BENCHMARK, "Evaluation of the current board state " + std::to_string(BENCHMARK_EVALUATION_COUNT) + " times.");
    timer.start();
    for (int i = 0; i < BENCHMARK_EVALUATION_COUNT; i++)
        engine.evaluatePosition();
    timer.stop(BENCHMARK);
}

void UserInterface::startSearch()
{
    log(CHESS_BOARD, "Starting search");
    Move best_move = engine.search();
    if (best_move == NULL_MOVE)
        log(SEARCH, "No legal moves for current board state.");
}

void UserInterface::startPerft()
{
    log(PERFT, "Starting perft");
    timer.start();
    uint64 nodes = engine.perft();
    timer.stop(PERFT);
    log(PERFT, "Nodes searched: " + std::to_string(nodes));
    if (expected_perft)
    {
        uint64 difference = nodes > expected_perft ? nodes - expected_perft : expected_perft - nodes;
        assert(nodes == expected_perft, "Expected: " + std::to_string(expected_perft) + "\nDiff: " + std::to_string(difference));
    }
}

void UserInterface::startUCI()
{
    log(UI, "ChessEngine started in UCI mode");
    UCI uci = UCI(engine);
    uci.start();
}

void UserInterface::startPlayerVsPlayer()
{
    log(UI, "ChessEngine started in player vs player mode.");
    printGameState();
    MoveList moves = engine.getLegalMoves();
    log(UI, "Enter 'p' to print legal moves or 'u' to undo the last move.");

    do
    {
        Move move = getMove(true);
        applyAndTrackMove(move);
        printGameState();
        moves = engine.getLegalMoves();
    } while (!engine.isGameOver());
    log(UI, engine.getGameState());
}

void UserInterface::startPlayerVsEngine()
{
    log(UI, "ChessEngine started in player vs engine mode.");
    printGameState();
    MoveList moves = engine.getLegalMoves();
    bool player_turn = promptForPlayerColor() == engine.getBoard().white_to_move;
    log(UI, (player_turn ? "You start.\n" : "The AI starts.\n"));
    log(UI, "Enter 'p' to print legal moves or 'u' to undo the last move.");

    do
    {
        Move move = getMove(player_turn);
        player_turn = !player_turn;
        applyAndTrackMove(move, true);
        printGameState();
        moves = engine.getLegalMoves();
    } while (!engine.isGameOver());
    log(UI, engine.getGameState());
}

void UserInterface::startEngineVsEngine()
{
    log(UI, "ChessEngine started in engine vs engine mode.");
    printGameState();
    MoveList moves = engine.getLegalMoves();
    std::string input;

    do
    {
        Move move = getMove(false);
        log(UI, "Press Enter to apply the chosen move.");
        input = readLineFromStdin();
        applyAndTrackMove(move);
        printGameState();
        moves = engine.getLegalMoves();
    } while (!engine.isGameOver());
    log(UI, engine.getGameState());
}

Move UserInterface::getMove(bool is_player_turn)
{
    if (is_player_turn)
        return promptForLegalMove(engine.getLegalMoves());
    if (use_opening_book)
    {
        Move move = opening_book.getBookMoveIfAvailable(engine.getBoard().hash);
        if (move != NULL_MOVE)
            return move;
    }
    return engine.search();
}

void UserInterface::applyAndTrackMove(Move move, bool play_vs_engine)
{
    if (move.isUndo())
    {
        assert(!move_history.empty(), "No move to undo");
        // additionally undo the AI's move if playing against the engine
        if (play_vs_engine && !move_history.empty())
        {
            move = move_history.top();
            move_history.pop();
            engine.unmakeMove(move);
        }
        move = move_history.top();
        move_history.pop();
        engine.unmakeMove(move);
        return;
    }

    move_history.push(move);
    engine.makeMove(move);
}

Move UserInterface::promptForLegalMove(MoveList legal_moves)
{
    std::string input;
    while (true)
    {
        std::cout << "Enter your move: ";
        input = readLineFromStdin();

        // print legal moves
        if (input[0] == 'p')
        {
            std::cout << "Legal moves: " << legal_moves.size << std::endl;
            for (int i = 0; i < legal_moves.size; i++)
                std::cout << "\t" << legal_moves.moves[i].toString() << std::endl;
            continue;
        }

        // undo previous move
        if (input[0] == 'u')
        {
            if (!move_history.empty())
                return UNDO_MOVE;
            print("No move to undo");
            continue;
        }

        // skip invalid moves
        if (input[0] < 'a' || input[0] > 'h' || input[1] < '1' || input[1] > '8' || input[2] < 'a' || input[2] > 'h' || input[3] < '1' || input[3] > '8')
        {
            print("Invalid move. Please enter a move in the format: 'a2c3'");
            continue;
        }

        // return move if legal else skip
        Position from = 8 * (input[1] - '1') + (input[0] - 'a');
        Position to = 8 * (input[3] - '1') + (input[2] - 'a');
        for (int i = 0; i < legal_moves.size; i++)
        {
            Move legal_move = legal_moves.moves[i];
            if (legal_move.from == from && legal_move.to == to)
            {
                if (legal_move.isPromotion())
                    legal_move.promotion = promptForPromotionChoice();
                return legal_move;
            }
        }
        print("Illegal move. Please enter a legal move.");
    }
}

bool UserInterface::promptForPlayerColor()
{
    std::string input;
    while (true)
    {
        print("Enter your color: ", false);
        input = readLineFromStdin();

        if (input[0] == 'b' || input[0] == 'B')
        {
            print("You chose black.");
            return false;
        }

        if (input[0] == 'w' || input[0] == 'W')
        {
            print("You chose white.");
            return true;
        }

        print("Invalid color. Please enter 'w' for white or 'b' for black.");
    }
}

Piece UserInterface::promptForPromotionChoice()
{
    print("Choose promotion piece: ");
    print("Q - Queen");
    print("R - Rook");
    print("B - Bishop");
    print("N - Knight");

    Piece choice;
    while (true)
    {
        std::cin >> choice;
        switch (choice)
        {
        case WHITE_QUEEN_SYMBOL:
        case BLACK_QUEEN_SYMBOL:
            return PROMOTE_TO_QUEEN;
        case WHITE_ROOK_SYMBOL:
        case BLACK_ROOK_SYMBOL:
            return PROMOTE_TO_ROOK;
        case WHITE_BISHOP_SYMBOL:
        case BLACK_BISHOP_SYMBOL:
            return PROMOTE_TO_BISHOP;
        case WHITE_KNIGHT_SYMBOL:
        case BLACK_KNIGHT_SYMBOL:
            return PROMOTE_TO_KNIGHT;
        default:
            break;
        }
        print("Invalid choice. Please enter Q, R, B or N.");
    }
}

void UserInterface::parseParameters(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-m" && i + 1 < argc)
            mode = argv[++i][0];
        else if (std::string(argv[i]) == "-f" && i + 1 < argc)
            engine.setPosition(argv[++i]);
        else if (std::string(argv[i]) == "-p" && i + 1 < argc)
            engine.setSearchDepth(std::stoi(argv[++i]));
        else if (std::string(argv[i]) == "-e" && i + 1 < argc)
            expected_perft = std::stoull(argv[++i]);
        else if (std::string(argv[i]) == "-t" && i + 1 < argc)
            engine.getTimeManagement().configure(std::stoull(argv[++i]));
        else if (std::string(argv[i]) == "-o" && i + 1 < argc)
            opening_book.enabled = opening_book.initializeBook(argv[++i]);
        else if (std::string(argv[i]) == "-h")
        {
            printHelp(argv[0]);
            exit(0);
        }
        else
        {
            std::cout << "Unknown option: " << argv[i] << std::endl;
            printHelp(argv[0]);
            exit(1);
        }
    }
}

void UserInterface::printHelp(const std::string &executable_name)
{
    std::cout << "Usage: " << executable_name << " [-m mode] [-f FEN] [-p ply] [-h]\n"
              << "  -m mode: Set the engine mode (" << AVAILABLE_MODES << ")\n"
              << "  -f FEN: Start the game with the given FEN string\n"
              << "  -p ply: Run perft/search with the given ply/depth/number of half moves)\n"
              << "  -t: Set search time (in milliseconds)\n"
              << "  -o: opening book file path (disabled by default)\n"
              << "  -h: Show this help message" << std::endl;
}

void UserInterface::printGameState()
{
    Score score = engine.evaluatePosition();
    log(UI, "Board Evaluation: " + std::to_string(score));
    Board::printGameState(engine.getBoard());
}

int main(int argc, char *argv[])
{
    UserInterface ui;
    ui.parseParameters(argc, argv);
    ui.start();
    exit(0);
}
