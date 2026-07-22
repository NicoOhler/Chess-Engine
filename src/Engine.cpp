#include "Engine.h"

GameState Engine::getGameState(MoveList moves)
{
    if (board.half_move_clock >= HALF_MOVE_CLOCK_LIMIT)
        return DRAW;
    if (!moves.empty())
        return IN_PROGRESS;
    Position king_square = getRightmostSetBit(board.white_to_move ? board.white_king : board.black_king);
    return move_generator.squareUnderAttack(board, king_square, board.white_to_move) ? CHECKMATE : DRAW;
}

uint64 Engine::perft(int depth, bool divide)
{
    MoveList legal_moves = move_generator.generateLegalMoves(board);
    if (depth == 1)
        return legal_moves.size;

    uint64 total_nodes = 0;
    for (int i = 0; i < legal_moves.size; i++)
    {
        Move move = legal_moves.moves[i];
        move_generator.makeMove(board, move);
        uint64 nodes = perft(depth - 1);
        move_generator.unmakeMove(board, move);
        if (divide)
            log(PERFT, getSquareName(move.from) + getSquareName(move.to) + ": " + std::to_string(nodes));
        total_nodes += nodes;
    }
    return total_nodes;
}

Score Engine::evaluateBoard()
{
    evaluated_nodes++;
    return eval.evaluateBoard(board);
}

// iterative deepening
Move Engine::search()
{
    evaluated_nodes = 0;
    best_root_score = NEG_INFINITY;
    best_root_move = NULL_MOVE;

    timer.start();
    for (int depth = 1; depth <= MAX_SEARCH_DEPTH && timer.timeLeft(); depth++)
    {
        best_root_score = negamax_search(0, depth, NEG_INFINITY, POS_INFINITY);
        log(SEARCH, "Depth " + std::to_string(depth) + " best move " + best_root_move.toString() +
                        " score " + std::to_string(best_root_score));
    }

    if (timer.timeLeft())
        timer.stop(SEARCH);
    else
        log(SEARCH, "Search interrupted due to time limit.");

    log(SEARCH, "Search score: " + std::to_string(best_root_score));
    log(SEARCH, "Evaluated nodes: " + std::to_string(evaluated_nodes));
    assert(best_root_move != NULL_MOVE, "No best move found");
    return best_root_move;
}

void Engine::calculateMoveScores(MoveList &moves)
{
    // extract best move from transposition table if available
    TranspositionEntry *entry = transposition_table.probe(board.hash);
    Move best_move = entry != nullptr ? entry->best_move : NULL_MOVE;

    for (int i = 0; i < moves.size; i++)
    {
        Move move = moves.moves[i];
        // search previously best move first
        if (move == best_move)
            moves.scores[i] = POS_INFINITY;

        // promotions
        else if (move.promotion)
            moves.scores[i] = getPieceValue(move.promotion) + 20000;

        // Most Valuable Victim – Least Valuable Aggressor
        // i.e., prioritize captures of high value with low value pieces
        else if (move.captured_piece != EMPTY)
            moves.scores[i] = getPieceValue(move.captured_piece) - getPieceValue(move.piece) + 10000;

        // todo add killer moves, history later on
        else
            moves.scores[i] = 0;
    }

    if (ENABLE_MOVE_SORTING)
        sortMoves(moves);
}

void Engine::sortMoves(MoveList &moves)
{
    // insertion sort seems best for small arrays
    for (int i = 1; i < moves.size; i++)
    {
        Move key_move = moves.moves[i];
        Score key_score = moves.scores[i];
        int j = i - 1;

        while (j >= 0 && moves.scores[j] < key_score)
        {
            moves.moves[j + 1] = moves.moves[j];
            moves.scores[j + 1] = moves.scores[j];
            j--;
        }
        moves.moves[j + 1] = key_move;
        moves.scores[j + 1] = key_score;
    }
}

Move Engine::pickBestMove(MoveList &moves)
{
    int best_move_index = -1;
    Score best_score = NEG_INFINITY;

    for (int i = 0; i < moves.size; i++)
        if (moves.scores[i] > best_score)
        {
            best_score = moves.scores[i];
            best_move_index = i;
        }

    // mark selected move as used => ensure it is not picked again
    if (best_move_index != -1)
        moves.scores[best_move_index] = NEG_INFINITY;

    return moves.moves[best_move_index];
}

// negamax search with alpha beta pruning and quiescence
Score Engine::negamax_search(int depth, int remaining_depth, Score lower_bound, Score upper_bound)
{
    // use results from transposition table if possible
    Score original_lower_bound = lower_bound;
    /*
    TranspositionEntry *entry = transposition_table.probe(board.hash);
    if (entry != nullptr && entry->depth >= remaining_depth)
    {
        // immediately return score if already computed
        bool below_lower_bound = (entry->type == EXACT || entry->type == UPPER_BOUND) && entry->score <= lower_bound;
        bool above_upper_bound = (entry->type == EXACT || entry->type == LOWER_BOUND) && entry->score >= upper_bound;
        if (below_lower_bound || above_upper_bound)
        return entry->score;

        // tighten lower bound and upper bound
        if (entry->type == LOWER_BOUND && entry->score > lower_bound)
        lower_bound = entry->score;
        if (entry->type == UPPER_BOUND && entry->score < upper_bound)
        upper_bound = entry->score;

        if (lower_bound >= upper_bound)
        return entry->score;
    }
    */

    // ? maybe store quiescence, checkmates and draws in transposition table?
    // return evaluation for leaf nodes (max depth reached)
    BoundType type = EXACT;
    if (remaining_depth == 0)
        return evaluateBoard();
    // return quiescence(lower_bound, upper_bound);

    MoveList legal_moves = move_generator.generateLegalMoves(board);
    GameState game_state = getGameState(legal_moves);
    // add depth to prefer fast games
    // ? could this mess with the transition table? => add depth back when storing in table
    if (game_state == CHECKMATE)
        return -MATE_VALUE + depth;
    if (game_state == DRAW)
        return DRAW_VALUE + depth;

    // evaluate moves until pruning possible
    calculateMoveScores(legal_moves);
    Score best_score = NEG_INFINITY;
    Move best_move;
    for (int i = 0; i < legal_moves.size; i++)
    {
        Move move = ENABLE_MOVE_SORTING ? legal_moves.moves[i] : pickBestMove(legal_moves);
        makeMove(move);
        // swap and negate lower/upper bounds since the opponent tries to minimize our score
        Score score = -negamax_search(depth + 1, remaining_depth - 1, -upper_bound, -lower_bound);
        unmakeMove(move);

        // keep track of best move
        if (score > best_score)
        {
            best_score = score;
            best_move = move;

            // update lower bound if exceeded
            if (score > lower_bound)
            {
                lower_bound = score;
                if (depth == 0 && score > best_root_score)
                    best_root_move = move;
            }
        }

        // prune if upper bound exceeded
        // i.e., opponent will pick another move that is better
        if (upper_bound <= lower_bound)
            break;

        // interrupt search if time is up
        if (!timer.timeLeft())
            return best_score;
    }

    if (best_score <= original_lower_bound)
        type = UPPER_BOUND;
    else if (best_score >= upper_bound)
        type = LOWER_BOUND;
    transposition_table.store(board.hash, best_score, best_move, remaining_depth, type);
    return best_score;
}

// ? maybe make use of transposition table?
Score Engine::quiescence(Score lower_bound, Score upper_bound, int depth)
{
    // use static evaluation as baseline
    // in case no further captures are possible
    Score stand_pat = evaluateBoard();
    if (stand_pat >= upper_bound)
        return upper_bound;
    if (stand_pat > lower_bound)
        lower_bound = stand_pat;

    // return if max depth exceeded or if current position is really bad (delta pruning)
    if (depth >= MAX_QUIESCENCE_DEPTH || stand_pat + DELTA_VALUE < lower_bound)
        return stand_pat;

    // further examine captures, checks and promotions
    MoveList legal_moves = move_generator.generateLegalMoves(board, true);
    calculateMoveScores(legal_moves);
    for (int i = 0; i < legal_moves.size; i++)
    {
        Move move = pickBestMove(legal_moves);
        // todo also consider checks
        if (move.captured_piece == EMPTY && move.promotion == EMPTY)
            continue;

        makeMove(move);
        Score score = -quiescence(-upper_bound, -lower_bound, depth + 1);
        unmakeMove(move);

        if (score >= upper_bound)
            return upper_bound;
        if (score > lower_bound)
            lower_bound = score;

        if (!timer.timeLeft())
            return lower_bound;
    }

    return lower_bound;
}

void Engine::initializeStartPosition(std::string fen)
{
    board = generateBoardFromFEN(fen);
    log(CHESS_BOARD, "Initialized board with FEN: " + fen);
    board.hash = zobrist.computeInitialHash(board);
}

void Engine::makeMove(Move move)
{
    move_generator.makeMove(board, move);
    zobrist.updateHash(move, board);
}

void Engine::unmakeMove(Move move)
{
    zobrist.updateHash(move, board);
    move_generator.unmakeMove(board, move);
}

// getter and setter
void Engine::setTimeLimit(Milliseconds time_limit)
{
    timer.limit = time_limit;
}

Board Engine::getBoard()
{
    return board; // copy
}

MoveList Engine::getLegalMoves()
{
    return move_generator.generateLegalMoves(board);
}