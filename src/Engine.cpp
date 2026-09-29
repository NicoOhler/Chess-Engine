#include "Engine.h"

GameState Engine::getGameState(MoveList moves)
{
    if (board.half_move_clock >= HALF_MOVE_CLOCK_LIMIT)
        return DRAW;
    if (!moves.empty())
        return IN_PROGRESS;
    Position king_square = getRightmostSetBit(board.white_to_move ? board.white_king : board.black_king);
    return move_generator.squareUnderAttack(board, king_square, !board.white_to_move) ? CHECKMATE : DRAW;
}

uint64 Engine::perft(int remaining_depth, bool divide)
{
    MoveList legal_moves = move_generator.generateLegalMoves(board);
    if (remaining_depth == 1)
        return legal_moves.size;

    uint64 total_nodes = 0;
    for (int i = 0; i < legal_moves.size; i++)
    {
        Move move = legal_moves.moves[i];
        move_generator.makeMove(board, move);
        uint64 nodes = perft(remaining_depth - 1);
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
    Score best_root_score = NEG_INFINITY;
    Move best_root_move = NULL_MOVE;

    timer.start();
    for (int depth = 1; depth <= MAX_SEARCH_DEPTH && timer.timeLeft(); depth++)
    {
        best_move_of_iteration = NULL_MOVE;
        best_score_of_iteration = negamax_search(0, depth, NEG_INFINITY, POS_INFINITY);
        log(SEARCH_DEPTHS, "Depth " + std::to_string(depth) + " best move " + best_move_of_iteration.toString() +
                               " score " + std::to_string(best_score_of_iteration));

        // throw away partial results if out of time
        if (timer.timeLeft())
        {
            best_root_score = best_score_of_iteration;
            best_root_move = best_move_of_iteration;
        }
    }

    if (timer.timeLeft())
        timer.stop(SEARCH);
    else
        log(SEARCH, "Search interrupted due to time limit.");

    log(SEARCH, "Search score: " + std::to_string(best_root_score));
    log(SEARCH, "Suggested move: " + best_root_move.toString());
    log(SEARCH, "Evaluated nodes: " + std::to_string(evaluated_nodes));
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
        // search previously best move first, which will likely cause a beta cutoff
        if (move == best_move)
            moves.scores[i] = POS_INFINITY;
        // then promotions
        else if (move.promotion)
            moves.scores[i] = getPieceValue(move.promotion) + PROMOTION_VALUE;
        // then captures according to Most Valuable Victim – Least Valuable Attacker
        // i.e., prioritize captures of high value with low value pieces
        else if (move.captured_piece != EMPTY)
            moves.scores[i] = getPieceValue(move.captured_piece) - getPieceValue(move.piece) + CAPTURE_VALUE;
        // then quiet non-capture moves
        else
            moves.scores[i] = 0;
    }
}

Move Engine::pickBestMove(MoveList &moves)
{
    int best_move_index = 0;
    Score best_score = moves.scores[0];

    for (int i = 1; i < moves.size; i++)
    {
        if (moves.scores[i] == USED_MOVE)
            continue;
        if (moves.scores[i] > best_score)
        {
            best_score = moves.scores[i];
            best_move_index = i;
        }
    }

    moves.scores[best_move_index] = USED_MOVE;
    return moves.moves[best_move_index];
}

Score Engine::negamax_search(int ply, int remaining_depth, Score lower_bound, Score upper_bound)
{
    // use results from transposition table if possible
    TranspositionEntry *entry = transposition_table.probe(board.hash);
    if (entry != nullptr && entry->remaining_depth >= remaining_depth)
    {
        log(TRANSPOSITION_TABLE_MATCH, "Found position in transposition table");
        Score retrieved_score = computeOriginalScore(entry->score, ply);
        // immediately return score if already computed
        bool below_lower_bound = (entry->type == EXACT || entry->type == UPPER_BOUND) && retrieved_score <= lower_bound;
        bool above_upper_bound = (entry->type == EXACT || entry->type == LOWER_BOUND) && retrieved_score >= upper_bound;
        if (below_lower_bound || above_upper_bound)
        {
            log(TRANSPOSITION_TABLE_MATCH, "Prune tree thanks to transposition table");
            return retrieved_score;
        }

        // tighten lower bound and upper bound
        if (entry->type == LOWER_BOUND && retrieved_score > lower_bound)
            lower_bound = retrieved_score;
        if (entry->type == UPPER_BOUND && retrieved_score < upper_bound)
            upper_bound = retrieved_score;

        if (lower_bound >= upper_bound)
        {
            log(TRANSPOSITION_TABLE_MATCH, "Prune tree thanks to transposition table");
            return retrieved_score;
        }
    }

    // return evaluation for leaf nodes (max depth reached)
    if (remaining_depth == 0)
        return quiescence(lower_bound, upper_bound, ply);
    // return evaluateBoard();

    MoveList legal_moves = move_generator.generateLegalMoves(board);
    GameState game_state = getGameState(legal_moves);
    // add ply to prefer fast games
    if (game_state == CHECKMATE)
        return -MATE_VALUE + ply;
    // do not add ply to avoid drawn out draws
    if (game_state == DRAW)
        return DRAW_VALUE;

    // evaluate moves until pruning possible
    BoundType type = EXACT;
    Score original_lower_bound = lower_bound;
    calculateMoveScores(legal_moves);
    Score best_score = NEG_INFINITY;
    Move best_move = NULL_MOVE;
    for (int i = 0; i < legal_moves.size; i++)
    {
        Move move = pickBestMove(legal_moves);
        makeMove(move);
        // swap and negate lower/upper bounds since the opponent tries to minimize our score
        Score score = -negamax_search(ply + 1, remaining_depth - 1, -upper_bound, -lower_bound);
        unmakeMove(move);

        // keep track of best move
        if (score > best_score)
        {
            best_score = score;
            best_move = move;
            if (ply == 0)
                best_move_of_iteration = move;

            // update lower bound if exceeded
            if (score > lower_bound)
                lower_bound = score;
        }

        // prune if upper bound exceeded
        // i.e., opponent will pick another move that is better
        if (upper_bound <= lower_bound)
            break;

        // interrupt search if time is up
        // if (!timer.timeLeft())
        //     return best_score;
    }

    if (best_score <= original_lower_bound)
        type = UPPER_BOUND;
    else if (best_score >= upper_bound)
        type = LOWER_BOUND;

    Score transposition_score = computeTranspositionScore(best_score, ply);
    transposition_table.store(board.hash, transposition_score, best_move, remaining_depth, type);
    return best_score;
}

Score Engine::quiescence(Score lower_bound, Score upper_bound, int ply)
{
    // use static evaluation as baseline
    // in case no further captures are possible
    Score stand_pat = evaluateBoard();
    if (stand_pat >= upper_bound)
        return upper_bound;
    if (stand_pat > lower_bound)
        lower_bound = stand_pat;

    // return if max depth exceeded or if current position is really bad (delta pruning)
    if (ply >= MAX_QUIESCENCE_DEPTH || stand_pat + DELTA_VALUE < lower_bound)
        return stand_pat;

    // further examine captures, checks and promotions
    MoveList legal_moves = move_generator.generateLegalMoves(board, true);
    calculateMoveScores(legal_moves);
    for (int i = 0; i < legal_moves.size; i++)
    {
        Move move = pickBestMove(legal_moves);
        if (move.captured_piece == EMPTY && move.promotion == EMPTY)
            continue;

        makeMove(move);
        Score score = -quiescence(-upper_bound, -lower_bound, ply + 1);
        unmakeMove(move);

        if (score >= upper_bound)
            return upper_bound;
        if (score > lower_bound)
            lower_bound = score;

        // if (!timer.timeLeft())
        //     return lower_bound;
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
    log(ENGINE_SETTINGS, "Set time limit to " + std::to_string(time_limit));
}

Board Engine::getBoard()
{
    return board; // copy
}

MoveList Engine::getLegalMoves()
{
    return move_generator.generateLegalMoves(board);
}