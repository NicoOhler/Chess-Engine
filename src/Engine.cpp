#include "Engine.h"

GameState Engine::getGameState(MoveList moves)
{
    if (board.half_move_clock >= HALF_MOVE_CLOCK_LIMIT)
        return DRAW;
    if (!moves.empty())
        return IN_PROGRESS;
    return move_generator.isKingSafe(board, board.white_to_move) ? DRAW : CHECKMATE;
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
        Score best_score_of_iteration = pv_search(0, depth, NEG_INFINITY, POS_INFINITY);
        Move best_move_of_iteration = transposition_table.probe(board.hash)->best_move;
        log(SEARCH_DEPTHS, "Depth " + std::to_string(depth) + " best move " + best_move_of_iteration.toString() +
                               " score " + std::to_string(best_score_of_iteration));

        // throw away partial results if out of time
        if (timer.timeLeft())
        {
            best_root_score = best_score_of_iteration;
            best_root_move = best_move_of_iteration;
        }
    }

    timer.stop(SEARCH);
    if (!timer.timeLeft())
        log(SEARCH, "Search interrupted due to time limit.");

    log(SEARCH, "Search score: " + std::to_string(best_root_score));
    log(SEARCH, "Suggested move: " + best_root_move.toString());
    log(SEARCH, "Evaluated nodes: " + std::to_string(evaluated_nodes));
    return best_root_move;
}

void Engine::calculateMoveScores(MoveList &moves, TranspositionEntry *entry)
{
    // retrieve best move from transposition table if available
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

// Principal Variation Search (PVS)
Score Engine::pv_search(int ply, int remaining_depth, Score lower_bound, Score upper_bound)
{
    Score original_lower_bound = lower_bound;
    Score original_upper_bound = upper_bound;

    // use results from transposition table if possible
    TranspositionEntry *entry = transposition_table.probe(board.hash);
    if (entry != nullptr)
    {
        // reuse previously computed score if it was computed with sufficient depth
        if (entry->remaining_depth >= remaining_depth)
        {
            log(TRANSPOSITION_TABLE_MATCH, "Found position in transposition table");
            Score retrieved_score = computeOriginalScore(entry->score, ply);

            // immediately return score if already computed
            if (entry->type == EXACT)
                return retrieved_score;
            // tighten lower bound and upper bound
            if (entry->type == LOWER_BOUND && retrieved_score > lower_bound)
                lower_bound = retrieved_score;
            if (entry->type == UPPER_BOUND && retrieved_score < upper_bound)
                upper_bound = retrieved_score;
            if (lower_bound >= upper_bound)
                return retrieved_score;
        }
    }

    // return evaluation for leaf nodes (max depth reached)
    if (remaining_depth == 0)
        return quiescence(lower_bound, upper_bound, ply);

    // generate and evaluate moves until pruning possible
    MoveList legal_moves = move_generator.generateLegalMoves(board);
    GameState game_state = getGameState(legal_moves);
    if (game_state == CHECKMATE)
        // add ply to prefer fast games
        return -MATE_VALUE + ply;
    if (game_state == DRAW)
        // do not add ply to avoid drawn out draws
        return DRAW_VALUE;

    calculateMoveScores(legal_moves, entry);
    Move move, best_move = NULL_MOVE;
    Score score, best_score = NEG_INFINITY;
    bool null_window = false;
    for (int i = 0; i < legal_moves.size; i++)
    {
        // interrupt search if time is up
        // if (!timer.timeLeft())
        //     return best_score;

        // prune if upper bound exceeded
        // i.e., opponent will pick another move that is better
        if (upper_bound <= lower_bound)
            break;

        // explore retrieved move first if available, then explore moves in order of score
        if (i == 0 && entry != nullptr)
        {
            move = entry->best_move;
            move_generator.markMoveAsUsed(legal_moves, move);
            // todo move needs to be legal (to avoid hash collisions)
            // ? why does exploring this move before move generation not make search faster
        }
        else
            move = move_generator.pickBestMove(legal_moves);
        makeMove(move);
        // search with null window (alpha, alpha + 1) to hopefully cause a beta cutoff
        if (null_window)
        {
            score = -pv_search(ply + 1, remaining_depth - 1, -lower_bound - 1, -lower_bound);
            // research with full window (alpha, beta) if no beta cutoff occurred
            if (score > lower_bound && score < upper_bound)
                score = -pv_search(ply + 1, remaining_depth - 1, -upper_bound, -lower_bound);
        }
        else // search with full window precision (i.e., regular negamax search)
            score = -pv_search(ply + 1, remaining_depth - 1, -upper_bound, -lower_bound);
        unmakeMove(move);

        // keep track of best move
        if (score > best_score)
        {
            best_score = score;
            best_move = move;
        }

        // update lower bound if exceeded
        if (score > lower_bound)
        {
            lower_bound = score;
            null_window = true;
        }
    }

    // store best move in transposition table
    BoundType type = transposition_table.determineBoundType(best_score, original_lower_bound, original_upper_bound);
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
    TranspositionEntry *entry = transposition_table.probe(board.hash);
    calculateMoveScores(legal_moves, entry);
    for (int i = 0; i < legal_moves.size; i++)
    {
        Move move = move_generator.pickBestMove(legal_moves);
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

bool Engine::makeMoveIfLegal(Move move)
{
    bool king_color_before_move = board.white_to_move;
    makeMove(move);
    if (move_generator.isKingSafe(board, king_color_before_move))
        return true;
    unmakeMove(move);
    return false;
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