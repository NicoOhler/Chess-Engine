#include "Evaluation.h"

// todo replace with actual NNUE evaluation
Score Evaluation::evaluateBoard(Board board)
{
    Score material = evaluateMaterial(board);
    Score mobility = evaluateMobility(board);
    Score pawn_structure = evaluatePawnStructure(board);
    Score perspective = board.white_to_move ? 1 : -1;
    return perspective * (material + mobility + pawn_structure);
}

Score Evaluation::evaluateMaterial(Board board)
{
    Score material = 0;
    material += countSetBits(board.white_pawns) * PAWN_VALUE;
    material += countSetBits(board.white_knights) * KNIGHT_VALUE;
    material += countSetBits(board.white_bishops) * BISHOP_VALUE;
    material += countSetBits(board.white_rooks) * ROOK_VALUE;
    material += countSetBits(board.white_queens) * QUEEN_VALUE;
    material += countSetBits(board.white_king) * KING_VALUE;

    material -= countSetBits(board.black_pawns) * PAWN_VALUE;
    material -= countSetBits(board.black_knights) * KNIGHT_VALUE;
    material -= countSetBits(board.black_bishops) * BISHOP_VALUE;
    material -= countSetBits(board.black_rooks) * ROOK_VALUE;
    material -= countSetBits(board.black_queens) * QUEEN_VALUE;
    material -= countSetBits(board.black_king) * KING_VALUE;
    log(EVALUATION, "Material: " + std::to_string(material));
    return material;
}

Score Evaluation::evaluateMobility(Board board)
{
    int number_of_own_moves = move_generator.generateLegalMoves(board).size;
    board.white_to_move = !board.white_to_move;
    int number_of_enemy_moves = move_generator.generateLegalMoves(board).size;
    board.white_to_move = !board.white_to_move;
    log(EVALUATION, "Mobility: " + std::to_string(number_of_own_moves - number_of_enemy_moves));
    return number_of_own_moves - number_of_enemy_moves;
}

Score Evaluation::evaluatePawnStructure(Board board)
{
    Bitboard own_pawns = board.white_to_move ? board.white_pawns : board.black_pawns;
    Bitboard enemy_pawns = board.white_to_move ? board.black_pawns : board.white_pawns;
    Direction direction = board.white_to_move ? UP : DOWN;
    Bitboard enemy_pawns_editable = enemy_pawns;
    Bitboard own_pawns_editable = own_pawns;

    // count own isolated, doubled and blocked pawns
    int own_isolated_pawns = 0, own_doubled_pawns = 0, own_blocked_pawns = 0;
    while (own_pawns_editable)
    {
        Position square = clearRightmostSetBit(own_pawns_editable);
        // detect isolated pawns (no friendly pawn on surrounding squares)
        if (!(own_pawns & (move_generator.king_moves[square])))
            own_isolated_pawns++;

        // detect doubled pawns (square above white/below black pawn occupied by own pawn)
        if (own_pawns & (1ULL << (square + direction)))
            own_doubled_pawns++;

        // ? is punishing doubled pawns twice (via blocked pawns) bad?
        // detect blocked pawns (square above white/below black pawn occupied by enemy pawn)
        if (enemy_pawns & (1ULL << (square + direction)))
            own_blocked_pawns++;
    }
    log(EVALUATION, "Own isolated pawns: " + std::to_string(own_isolated_pawns));
    log(EVALUATION, "Own doubled pawns: " + std::to_string(own_doubled_pawns));
    log(EVALUATION, "Own blocked pawns: " + std::to_string(own_blocked_pawns));

    // count enemy isolated, doubled and blocked pawns
    int enemy_isolated_pawns = 0, enemy_doubled_pawns = 0, enemy_blocked_pawns = 0;
    while (enemy_pawns_editable)
    {
        Position square = clearRightmostSetBit(enemy_pawns_editable);
        // detect isolated pawns (no friendly pawn on surrounding squares)
        if (!(enemy_pawns & (move_generator.king_moves[square])))
            enemy_isolated_pawns++;

        // detect doubled pawns (square above white/below black pawn occupied by own pawn)
        if (enemy_pawns & (1ULL << (square + direction)))
            enemy_doubled_pawns++;

        // detect blocked pawns (square above white/below black pawn occupied by enemy pawn)
        if (own_pawns & (1ULL << (square + direction)))
            enemy_blocked_pawns++;
    }
    log(EVALUATION, "Enemy isolated pawns: " + std::to_string(enemy_isolated_pawns));
    log(EVALUATION, "Enemy doubled pawns: " + std::to_string(enemy_doubled_pawns));
    log(EVALUATION, "Enemy blocked pawns: " + std::to_string(enemy_blocked_pawns));

    Score pawn_structure = (own_isolated_pawns - enemy_isolated_pawns) * ISOLATED_PAWN_VALUE;
    pawn_structure += (own_doubled_pawns - enemy_doubled_pawns) * DOUBLED_PAWN_VALUE;
    pawn_structure += (own_blocked_pawns - enemy_blocked_pawns) * BLOCKED_PAWN_VALUE;
    log(EVALUATION, "Pawn structure: " + std::to_string(pawn_structure));
    return pawn_structure;
}