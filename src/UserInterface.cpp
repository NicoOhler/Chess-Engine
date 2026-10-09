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
    case SEARCH_MODE:
        startSearch();
        break;
    default:
        std::cout << "Invalid mode. Use 'u' for UCI, 'c' for console, 'p' for perft or 's' for search." << std::endl;
        exit(1);
        break;
    }
}

void UserInterface::startSearch()
{
    engine.initializeStartPosition(fen);
    log(CHESS_BOARD, "Starting search with a time limit of " + std::to_string(search_time) + "ms");
    Move best_move = engine.search(ply);
    if (best_move == NULL_MOVE)
        log(SEARCH, "No legal moves for current board state.");
}

void UserInterface::startPerft()
{
    if (ply <= 0)
    {
        log(UI, "Invalid perft depth. Please specify a positive integer for the depth.");
        return;
    }
    engine.initializeStartPosition(fen);
    log(PERFT, "Starting perft with a depth of " + std::to_string(ply));
    timer.start();
    uint64 nodes = engine.perft(ply, divide);
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
    std::string command;
    while (std::getline(std::cin, command))
    {
        if (command == "uci")
        {
            std::cout << "id name Chess Engine" << std::endl;
            std::cout << "id author Nico Ohler" << std::endl;
            std::cout << "uciok" << std::endl;
        }
        else if (command == "isready")
            std::cout << "readyok" << std::endl;
        // position [ fen fenstring | startpos ] moves move1 ... movei
        else if (command.substr(0, 8) == "position")
        {
            std::string fen = command.substr(9);
            if (fen == "startpos")
                fen = START_FEN;
            engine.initializeStartPosition(fen);
        }
        else if (command.substr(0, 4) == "go ")
        {
            // parse depth or other parameters if needed
            startPerft();
        }
        else if (command == "quit")
            exit(0);
    }
}

void UserInterface::startPlayerVsPlayer()
{
    log(UI, "ChessEngine started in player vs player mode.");
    engine.initializeStartPosition(fen);
    printGameState();
    MoveList moves = engine.getLegalMoves();
    log(UI, "Enter 'p' to print legal moves or 'u' to undo the last move.");

    do
    {
        Move move = promptForLegalMove(moves);
        applyAndTrackMove(move);
        printGameState();
        moves = engine.getLegalMoves();
    } while (engine.getGameState(moves) == IN_PROGRESS);
    log(UI, (engine.getGameState(moves) == CHECKMATE ? "Checkmate" : "Draw"));
}

void UserInterface::startPlayerVsEngine()
{
    log(UI, "ChessEngine started in player vs engine mode.");
    engine.initializeStartPosition(fen);
    printGameState();
    MoveList moves = engine.getLegalMoves();
    bool ai_turn = promptForPlayerColor() != engine.getBoard().white_to_move;
    log(UI, (ai_turn ? "The AI starts.\n" : "You start.\n"));
    log(UI, "Enter 'p' to print legal moves or 'u' to undo the last move.");

    do
    {
        Move move = ai_turn ? engine.search(ply) : promptForLegalMove(moves);
        ai_turn = !ai_turn;
        applyAndTrackMove(move, true);
        printGameState();
        moves = engine.getLegalMoves();
    } while (engine.getGameState(moves) == IN_PROGRESS);
    log(UI, (engine.getGameState(moves) == CHECKMATE ? "Checkmate" : "Draw"));
}

void UserInterface::startEngineVsEngine()
{
    log(UI, "ChessEngine started in engine vs engine mode.");
    engine.initializeStartPosition(fen);
    printGameState();
    MoveList moves = engine.getLegalMoves();
    std::string input;

    do
    {
        Move move = engine.search(ply);
        log(UI, "Press Enter to apply the chosen move.");
        std::getline(std::cin, input);
        applyAndTrackMove(move);
        printGameState();
        moves = engine.getLegalMoves();
    } while (engine.getGameState(moves) == IN_PROGRESS);
    log(UI, (engine.getGameState(moves) == CHECKMATE ? "Checkmate" : "Draw"));
}

void UserInterface::applyAndTrackMove(Move move, bool play_vs_engine)
{
    if (move.piece == UNDO)
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
        std::getline(std::cin, input);

        // print legal moves
        if (input[0] == 'p')
        {
            std::cout << "Legal moves: " << legal_moves.size << std::endl;
            for (int i = 0; i < legal_moves.size; i++)
            {
                Move legal_move = legal_moves.moves[i];
                std::cout << "\t" << getSquareName(legal_move.from) << getSquareName(legal_move.to);
                if (legal_move.promotion)
                    std::cout << " (" << legal_move.promotion << ")";
                std::cout << std::endl;
            }
            continue;
        }

        // undo previous move
        if (input[0] == 'u')
        {
            if (!move_history.empty())
                return UNDO_MOVE;
            std::cout << "No move to undo" << std::endl;
            continue;
        }

        // skip invalid moves
        if (input[0] < 'a' || input[0] > 'h' || input[1] < '1' || input[1] > '8' || input[2] < 'a' || input[2] > 'h' || input[3] < '1' || input[3] > '8')
        {
            std::cout << "Invalid move. Please enter a move in the format: 'a2c3'" << std::endl;
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
                if (legal_move.promotion)
                    legal_move.promotion = promptForPromotionChoice();
                return legal_move;
            }
        }
        std::cout << "Illegal move. Please enter a legal move." << std::endl;
    }
}

bool UserInterface::promptForPlayerColor()
{
    std::string input;
    while (true)
    {
        std::cout << "Enter your color: ";
        std::getline(std::cin, input);

        if (input[0] == 'b' || input[0] == 'B')
        {
            std::cout << "You chose black." << std::endl;
            return false;
        }

        if (input[0] == 'w' || input[0] == 'W')
        {
            std::cout << "You chose white." << std::endl;
            return true;
        }

        std::cout << "Invalid color. Please enter 'w' for white or 'b' for black." << std::endl;
    }
}

Piece UserInterface::promptForPromotionChoice()
{
    std::cout << "Choose promotion piece: " << std::endl;
    std::cout << "Q - Queen" << std::endl;
    std::cout << "R - Rook" << std::endl;
    std::cout << "B - Bishop" << std::endl;
    std::cout << "N - Knight" << std::endl;

    Piece choice;
    while (true)
    {
        std::cin >> choice;
        switch (choice)
        {
        case WHITE_QUEEN_SYMBOL:
            return WHITE_QUEEN;
        case WHITE_ROOK_SYMBOL:
            return WHITE_ROOK;
        case WHITE_BISHOP_SYMBOL:
            return WHITE_BISHOP;
        case WHITE_KNIGHT_SYMBOL:
            return WHITE_KNIGHT;
        case BLACK_QUEEN_SYMBOL:
            return BLACK_QUEEN;
        case BLACK_ROOK_SYMBOL:
            return BLACK_ROOK;
        case BLACK_BISHOP_SYMBOL:
            return BLACK_BISHOP;
        case BLACK_KNIGHT_SYMBOL:
            return BLACK_KNIGHT;
        default:
            break;
        }
        std::cout << "Invalid choice. Please enter Q, R, B or N." << std::endl;
    }
}

void UserInterface::parseParameters(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (std::string(argv[i]) == "-m" && i + 1 < argc)
            mode = argv[++i][0];
        else if (std::string(argv[i]) == "-f" && i + 1 < argc)
            fen = argv[++i];
        else if (std::string(argv[i]) == "-p" && i + 1 < argc)
        {
            ply = std::stoi(argv[++i]);
            log(ENGINE_SETTINGS, "Set search ply/depth to " + std::to_string(ply));
        }
        else if (std::string(argv[i]) == "-e" && i + 1 < argc)
            expected_perft = std::stoull(argv[++i]);
        else if (std::string(argv[i]) == "-d")
            divide = true;
        else if (std::string(argv[i]) == "-t" && i + 1 < argc)
        {
            search_time = std::stoull(argv[++i]);
            engine.setTimeLimit(search_time);
        }
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

void UserInterface::printHelp(std::string executable_name)
{
    std::cout << "Usage: " << executable_name << " [-m mode] [-f FEN] [-p ply] [-d] [-h]\n"
              << "  -m mode: Set the engine mode (u for UCI, c for console, p for perft, e for engine self play)\n"
              << "  -f FEN: Start the game with the given FEN string\n"
              << "  -p ply: Run perft/search with the given ply/depth/number of half moves)\n"
              << "  -d: Divide perft results\n"
              << "  -t: Set search time (in milliseconds)\n"
              << "  -h: Show this help message" << std::endl;
}

void UserInterface::printGameState()
{
    Score score = engine.evaluateBoard();
    log(UI, "Board Evaluation: " + std::to_string(score));
    BitBoard::printGameState(engine.getBoard());
}

int main(int argc, char *argv[])
{
    UserInterface ui;
    ui.parseParameters(argc, argv);
    ui.start();
    exit(0);
}
