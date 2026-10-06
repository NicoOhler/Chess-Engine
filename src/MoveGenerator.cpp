#include "MoveGenerator.h"

MoveGenerator::MoveGenerator()
{
    initializePawnCaptureMasks();
    initializeKnightMoves();
    initializeBishopBlockers();
    initializeRookBlockers();
    initializeKingMoves();
    initializeRookBishopAttacks();
}

void MoveGenerator::initializePawnCaptureMasks()
{
    log(MOVE_GENERATOR, "Initializing pawn capture masks.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = square / 8;
        Position col = modPow2(square, 8);

        if (col != COL_A)
        {
            if (row != ROW_8)
                set(white_pawn_attack_left[square], square + UP_LEFT);
            if (row != ROW_1)
                set(black_pawn_attack_left[square], square + DOWN_LEFT);
        }

        if (col != COL_H)
        {
            if (row != ROW_8)
                set(white_pawn_attack_right[square], square + UP_RIGHT);
            if (row != ROW_1)
                set(black_pawn_attack_right[square], square + DOWN_RIGHT);
        }
    }
}

void MoveGenerator::initializeKnightMoves()
{
    log(MOVE_GENERATOR, "Initializing knight moves.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = square / 8;
        Position col = modPow2(square, 8);

        if (col > COL_B && row < ROW_8)
            set(knight_moves[square], square + 2 * LEFT + UP);

        if (col < COL_G && row < ROW_8)
            set(knight_moves[square], square + 2 * RIGHT + UP);

        if (col > COL_A && row < ROW_7)
            set(knight_moves[square], square + LEFT + 2 * UP);

        if (col < COL_H && row < ROW_7)
            set(knight_moves[square], square + RIGHT + 2 * UP);

        if (col > COL_B && row > ROW_1)
            set(knight_moves[square], square + 2 * LEFT + DOWN);

        if (col < COL_G && row > ROW_1)
            set(knight_moves[square], square + 2 * RIGHT + DOWN);

        if (col > COL_A && row > ROW_2)
            set(knight_moves[square], square + LEFT + 2 * DOWN);

        if (col < COL_H && row > ROW_2)
            set(knight_moves[square], square + RIGHT + 2 * DOWN);
    }
}

void MoveGenerator::initializeBishopBlockers()
{
    log(MOVE_GENERATOR, "Initializing bishop blockers.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = square / 8;
        Position col = modPow2(square, 8);

        if (row < ROW_8)
        {
            if (col < COL_H)
                for (Position i = square + UP_RIGHT; (i < NUM_SQUARES + DOWN && (modPow2(i, 8)) != COL_H); i += UP_RIGHT)
                    set(bishop_blockers[square], i);

            if (col > COL_A)
                for (Position i = square + UP_LEFT; (i < NUM_SQUARES + DOWN && (modPow2(i, 8)) != COL_A); i += UP_LEFT)
                    set(bishop_blockers[square], i);
        }

        if (row > ROW_1)
        {
            if (col < COL_H)
                for (Position i = square + DOWN_RIGHT; (i >= UP && (modPow2(i, 8)) != COL_H); i += DOWN_RIGHT)
                    set(bishop_blockers[square], i);

            if (col > COL_A)
                for (Position i = square + DOWN_LEFT; (i >= UP && (modPow2(i, 8)) != COL_A); i += DOWN_LEFT)
                    set(bishop_blockers[square], i);
        }

        assert(countSetBits(bishop_blockers[square]) == BISHOP_RELEVANT_SQUARES[square], "Bishop blockers count does not match relevant squares.");
    }
}

void MoveGenerator::initializeRookBlockers()
{
    log(MOVE_GENERATOR, "Initializing rook blockers.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = square / 8;
        Position col = modPow2(square, 8);

        if (col < COL_H)
            for (Position i = square + RIGHT; (modPow2(i, 8)) != COL_H; i += RIGHT)
                set(rook_blockers[square], i);

        if (col > COL_A)
            for (Position i = square + LEFT; (modPow2(i, 8)) != COL_A; i += LEFT)
                set(rook_blockers[square], i);

        for (Position i = square + UP; i < NUM_SQUARES + DOWN; i += UP)
            set(rook_blockers[square], i);

        for (Position i = square + DOWN; i >= UP; i += DOWN)
            set(rook_blockers[square], i);

        assert(countSetBits(rook_blockers[square]) == ROOK_RELEVANT_SQUARES[square], "Rook blockers count does not match relevant squares.");
    }
}

void MoveGenerator::initializeKingMoves()
{
    log(MOVE_GENERATOR, "Initializing king moves.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = square / 8;
        Position col = modPow2(square, 8);

        if (col != COL_H)
        {
            set(king_moves[square], square + RIGHT);
            if (row != ROW_8)
                set(king_moves[square], square + UP_RIGHT);
            if (row != ROW_1)
                set(king_moves[square], square + DOWN_RIGHT);
        }

        if (col != COL_A)
        {
            set(king_moves[square], square + LEFT);
            if (row != ROW_8)
                set(king_moves[square], square + UP_LEFT);
            if (row != ROW_1)
                set(king_moves[square], square + DOWN_LEFT);
        }
        if (row != ROW_8)
            set(king_moves[square], square + UP);

        if (row != ROW_1)
            set(king_moves[square], square + DOWN);
    }
}

void MoveGenerator::addPawnMoveWithPossiblePromotion(Board &board, MoveList &moves, Move move)
{
    // regular move
    if (move.to >= UP && move.to < NUM_SQUARES + DOWN)
    {
        moves.append(move);
        // log(PAWN_MOVE, "Found pawn move from " + getSquareName(move.from) + " to " + getSquareName(move.to) + ".");
        return;
    }

    // pawn promotion
    bool white_to_move = board.white_to_move;
    move.promotion = white_to_move ? WHITE_QUEEN : BLACK_QUEEN;
    moves.append(move);
    move.promotion = white_to_move ? WHITE_ROOK : BLACK_ROOK;
    moves.append(move);
    move.promotion = white_to_move ? WHITE_BISHOP : BLACK_BISHOP;
    moves.append(move);
    move.promotion = white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT;
    moves.append(move);
    // log(PAWN_MOVE, "Found pawn promotion from " + getSquareName(move.from) + " to " + getSquareName(move.to) + ".");
}

void MoveGenerator::addCastlingMoves(Board &board, MoveList &moves)
{
    bool white_to_move = board.white_to_move;
    Piece piece = white_to_move ? WHITE_KING : BLACK_KING;
    Position king_start = white_to_move ? WHITE_KING_START_POSITION : BLACK_KING_START_POSITION;
    Bitboard king_side_castling = white_to_move ? WHITE_KING_SIDE_CASTLING : BLACK_KING_SIDE_CASTLING;
    if (board.castling_rights & king_side_castling) // not possible if king or rook has moved
    {
        Bitboard king_side_blocked = board.occupied & king_side_castling;                        // squares between king and rook must be empty
        bool king_side_attacked = squaresUnderAttack(board, king_side_castling, !white_to_move); // squares in between must not be attacked
        if (!king_side_blocked && !king_side_attacked)
        {
            Position to = king_start + RIGHT * 2;
            moves.append(Move{king_start, to, piece, 0, king_side_castling});
            // log(KING_MOVE, "Found king side castling from " + getSquareName(king_start) + " to " + getSquareName(to) + ".");
        }
    }

    Bitboard queen_side_castling = white_to_move ? WHITE_QUEEN_SIDE_CASTLING : BLACK_QUEEN_SIDE_CASTLING;
    if (board.castling_rights & queen_side_castling)
    {
        Bitboard queen_side_blocked = board.occupied & queen_side_castling;
        Bitboard queen_side_not_under_check = white_to_move ? WHITE_QUEEN_SIDE_CASTLING_NOT_UNDER_CHECK : BLACK_QUEEN_SIDE_CASTLING_NOT_UNDER_CHECK;
        bool queen_side_attacked = squaresUnderAttack(board, queen_side_not_under_check, !white_to_move);
        if (!queen_side_blocked && !queen_side_attacked)
        {
            Position to = king_start + LEFT * 2;
            moves.append(Move{king_start, to, piece, 0, queen_side_castling});
            // log(KING_MOVE, "Found queen side castling from " + getSquareName(king_start) + " to " + getSquareName(to) + ".");
        }
    }
}

Bitboard MoveGenerator::getOccupancyVariation(int index, int relevant_bits, Bitboard moves)
{
    Bitboard occupancy = 0;
    for (int i = 0; i < relevant_bits; i++)
    {
        Position square = clearRightmostSetBit(moves);
        // if i-th bit is set in index
        // e.g 3 = 0011 => set 0th and 1st bit
        if (index & (1 << i))
            occupancy |= (1ULL << square);
    }

    return occupancy;
}

void MoveGenerator::initializeRookBishopAttacks()
{
    log(MOVE_GENERATOR, "Initializing rook and bishop attacks.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        int8 rook_relevant_squares = ROOK_RELEVANT_SQUARES[square];
        int8 bishop_relevant_squares = BISHOP_RELEVANT_SQUARES[square];
        int number_of_rook_attacks = 1 << rook_relevant_squares; // 2^rook_relevant_squares, number of occupancy variations for a square
        int number_of_bishop_attacks = 1 << bishop_relevant_squares;
        int8 rook_bits = NUM_SQUARES - rook_relevant_squares; // number of bits to shift to get magic index
        int8 bishop_bits = NUM_SQUARES - bishop_relevant_squares;

        for (int i = 0; i < number_of_rook_attacks; i++)
        {
            // ? is it possible to compute all occupancy variations for a square at once?
            Bitboard occupancy = getOccupancyVariation(i, rook_relevant_squares, rook_blockers[square]);
            int magic_index = (occupancy * ROOK_MAGICS[square]) >> rook_bits;
            rook_attacks[square][magic_index] = precomputeSlidingRookAttacks(square, occupancy);
        }

        for (int i = 0; i < number_of_bishop_attacks; i++)
        {
            Bitboard occupancy = getOccupancyVariation(i, bishop_relevant_squares, bishop_blockers[square]);
            int magic_index = (occupancy * BISHOP_MAGICS[square]) >> bishop_bits;
            bishop_attacks[square][magic_index] = precomputeSlidingBishopAttacks(square, occupancy);
        }
    }
}

Bitboard MoveGenerator::precomputeSlidingRookAttacks(Position square, Bitboard occupied)
{
    Bitboard attacks = 0;
    for (Position i = square + RIGHT; (modPow2(i, 8)) != COL_A; i += RIGHT)
    {
        BitBoard::set(attacks, i);
        if (BitBoard::isSet(occupied, i))
            break;
    }

    // need 1 >= 0 since modulo of negative numbers are negative in C
    for (Position i = square + LEFT; i >= 0 && (modPow2(i, 8)) != COL_H; i += LEFT)
    {
        BitBoard::set(attacks, i);
        if (BitBoard::isSet(occupied, i))
            break;
    }

    for (Position i = square + UP; i < NUM_SQUARES; i += UP)
    {
        BitBoard::set(attacks, i);
        if (BitBoard::isSet(occupied, i))
            break;
    }

    for (Position i = square + DOWN; i >= ROW_1; i += DOWN)
    {
        BitBoard::set(attacks, i);
        if (BitBoard::isSet(occupied, i))
            break;
    }

    return attacks;
}

Bitboard MoveGenerator::precomputeSlidingBishopAttacks(Position square, Bitboard occupied)
{
    Bitboard attacks = 0;
    for (Position i = square + UP_RIGHT; (i < NUM_SQUARES && (modPow2(i, 8)) != COL_A); i += UP_RIGHT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    for (Position i = square + UP_LEFT; (i < NUM_SQUARES && (modPow2(i, 8)) != COL_H); i += UP_LEFT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    for (Position i = square + DOWN_RIGHT; (i >= ROW_1 && (modPow2(i, 8)) != COL_A); i += DOWN_RIGHT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    for (Position i = square + DOWN_LEFT; (i >= ROW_1 && (modPow2(i, 8)) != COL_H); i += DOWN_LEFT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    return attacks;
}

MoveList MoveGenerator::generateLegalMoves(Board board, bool interesting_only)
{
    MoveList legal_moves;
    MoveList pseudo_legal_moves = generatePseudoLegalMoves(board, interesting_only);

    // simulate each move and ensure king (of player making a move) is not in check
    for (int i = 0; i < pseudo_legal_moves.size; i++)
    {
        Move move = pseudo_legal_moves.moves[i];
        // legal if new king position is safe
        bool king_color_before_move = board.white_to_move;
        UndoInfo undo = makeMove(board, move);
        if (isKingSafe(board, king_color_before_move))
            legal_moves.append(move);
        unmakeMove(board, move, undo);
    }

    return legal_moves;
}

MoveList MoveGenerator::generatePseudoLegalMoves(Board &board, bool interesting_only)
{
    MoveList moves;
    generatePawnMoves(board, moves, interesting_only);
    generateKnightMoves(board, moves, interesting_only);
    generateBishopMoves(board, moves, interesting_only);
    generateRookMoves(board, moves, interesting_only);
    generateQueenMoves(board, moves, interesting_only);
    generateKingMoves(board, moves, interesting_only);
    return moves;
}

bool MoveGenerator::isKingSafe(Board board, bool king_is_white)
{
    Position king_square = getRightmostSetBit(king_is_white ? board.white_king : board.black_king);
    return !squareUnderAttack(board, king_square, !king_is_white);
}

void MoveGenerator::generateKingMoves(Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Piece piece = white_to_move ? WHITE_KING : BLACK_KING;
    Bitboard king = white_to_move ? board.white_king : board.black_king;
    Bitboard own_pieces = white_to_move ? board.white_pieces : board.black_pieces;

    // regular moves and attacks
    Position from = getRightmostSetBit(king);
    Bitboard attacks = king_moves[from] & ~own_pieces;
    while (attacks)
    {
        Position to = clearRightmostSetBit(attacks);
        // focus only on captures for interesting_only
        if (interesting_only && !board.isSquareOccupied(to))
            continue;
        // move may not put king into check
        if (squareUnderAttack(board, to, !white_to_move))
            continue;
        moves.append(Move{from, to, piece});
        // log(KING_MOVE, "Found king move from " + getSquareName(from) + " to " + getSquareName(to) + ".");
    }

    // castling moves
    if (!interesting_only && !squareUnderAttack(board, from, !white_to_move))
        addCastlingMoves(board, moves);
}

void MoveGenerator::generatePawnMoves(Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Piece piece = white_to_move ? WHITE_PAWN : BLACK_PAWN;
    Bitboard pawns = white_to_move ? board.white_pawns : board.black_pawns;
    Bitboard *attack_right = white_to_move ? white_pawn_attack_right : black_pawn_attack_right;
    Bitboard *attack_left = white_to_move ? white_pawn_attack_left : black_pawn_attack_left;
    Bitboard start_row = white_to_move ? WHITE_PAWNS_START : BLACK_PAWNS_START;
    Bitboard enemies = white_to_move ? board.black_pieces : board.white_pieces;
    Direction direction = white_to_move ? UP : DOWN;
    Direction direction_right = white_to_move ? UP_RIGHT : DOWN_RIGHT;
    Direction direction_left = white_to_move ? UP_LEFT : DOWN_LEFT;

    // single move
    // right shift for white, left shift for black since negative shift is undefined
    Bitboard single_moves = white_to_move ? (pawns << UP) : (pawns >> UP);
    single_moves &= ~board.occupied;
    while (single_moves)
    {
        Position to = clearRightmostSetBit(single_moves);
        // skip non promotion moves for interesting_only
        if (interesting_only && to >= UP && to < NUM_SQUARES + DOWN)
            continue;
        Position from = to - direction;
        addPawnMoveWithPossiblePromotion(board, moves, Move{from, to, piece});
    }

    if (!interesting_only)
    {
        // double move
        Bitboard pawns_on_start_row = pawns & start_row;
        Bitboard pawns_blocked_by_pieces_one_square_in_front = (white_to_move ? (board.occupied >> UP) : (board.occupied << UP));
        Bitboard pawns_blocked_by_pieces_two_squares_in_front = (white_to_move ? (board.occupied >> (2 * UP)) : (board.occupied << (2 * UP)));
        Bitboard double_moves = pawns_on_start_row & ~pawns_blocked_by_pieces_one_square_in_front & ~pawns_blocked_by_pieces_two_squares_in_front;
        while (double_moves)
        {
            Position from = clearRightmostSetBit(double_moves);
            Position to = from + 2 * direction;
            moves.append(Move{from, to, piece});
            // log(PAWN_MOVE, "Found pawn move from " + getSquareName(from) + " to " + getSquareName(to) + ".");
        }
    }

    // for each pawn check attack masks
    while (pawns)
    {
        Position from = clearRightmostSetBit(pawns);

        // attack right
        Position to_right = from + direction_right;
        Bitboard attack = attack_right[from] & enemies;
        bool en_passant_right = (modPow2(from, 8) != COL_H && board.en_passant == to_right);
        if (attack)
            addPawnMoveWithPossiblePromotion(board, moves, Move{from, to_right, piece});
        else if (en_passant_right)
            moves.append(Move{from, to_right, piece});
        // log(PAWN_MOVE, "Found en passant move from " + getSquareName(from) + " to " + getSquareName(to_right) + ".");

        // attack left
        Position to_left = from + direction_left;
        attack = attack_left[from] & enemies;
        bool en_passant_left = (modPow2(from, 8) != COL_A && board.en_passant == to_left);
        if (attack)
            addPawnMoveWithPossiblePromotion(board, moves, Move{from, to_left, piece});
        else if (en_passant_left)
            moves.append(Move{from, to_left, piece});
        // log(PAWN_MOVE, "Found en passant move from " + getSquareName(from) + " to " + getSquareName(to_left) + ".");
    }
}

void MoveGenerator::generateKnightMoves(Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Piece piece = white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT;
    Bitboard knights = white_to_move ? board.white_knights : board.black_knights;
    Bitboard own_pieces = white_to_move ? board.white_pieces : board.black_pieces;

    while (knights)
    {
        Position from = clearRightmostSetBit(knights);
        Bitboard attacks = knight_moves[from] & ~own_pieces;
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            // focus only on captures for interesting_only
            if (interesting_only && !board.isSquareOccupied(to))
                continue;
            moves.append(Move{from, to, piece});
            // log(KNIGHT_MOVE, "Found knight move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

void MoveGenerator::generateBishopMoves(Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Piece piece = white_to_move ? WHITE_BISHOP : BLACK_BISHOP;
    Bitboard bishops = white_to_move ? board.white_bishops : board.black_bishops;
    Bitboard own_pieces = white_to_move ? board.white_pieces : board.black_pieces;

    // for each bishop add each attacked square to moves
    while (bishops)
    {
        Position from = clearRightmostSetBit(bishops);
        // i would love to write an insightful comment here, but i have no idea how magic bitboards work
        // after all, they are by definition magic ¯\_(ツ)_/¯
        int magic_index = ((bishop_blockers[from] & board.occupied) * BISHOP_MAGICS[from]) >> (NUM_SQUARES - BISHOP_RELEVANT_SQUARES[from]);
        Bitboard attacks = bishop_attacks[from][magic_index] & ~own_pieces;
        // add each square attacked by current bishop to moves
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            // focus only on captures for interesting_only
            if (interesting_only && !board.isSquareOccupied(to))
                continue;
            moves.append(Move{from, to, piece});
            // log(BISHOP_MOVE, "Found bishop move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

void MoveGenerator::generateRookMoves(Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Piece piece = white_to_move ? WHITE_ROOK : BLACK_ROOK;
    Bitboard rooks = white_to_move ? board.white_rooks : board.black_rooks;
    Bitboard own_pieces = white_to_move ? board.white_pieces : board.black_pieces;

    // for each rook add each attacked square to moves
    while (rooks)
    {
        Position from = clearRightmostSetBit(rooks);
        int magic_index = ((rook_blockers[from] & board.occupied) * ROOK_MAGICS[from]) >> (NUM_SQUARES - ROOK_RELEVANT_SQUARES[from]);
        Bitboard attacks = rook_attacks[from][magic_index] & ~own_pieces;
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            // focus only on captures for interesting_only
            if (interesting_only && !board.isSquareOccupied(to))
                continue;
            moves.append(Move{from, to, piece});
            // log(ROOK_MOVE, "Found rook move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

void MoveGenerator::generateQueenMoves(Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Piece piece = white_to_move ? WHITE_QUEEN : BLACK_QUEEN;

    // for each queen add each attacked square to moves
    Bitboard queens = white_to_move ? board.white_queens : board.black_queens;
    while (queens)
    {
        Position from = clearRightmostSetBit(queens);
        int rook_magic_index = ((rook_blockers[from] & board.occupied) * ROOK_MAGICS[from]) >> (NUM_SQUARES - ROOK_RELEVANT_SQUARES[from]);
        int bishop_magic_index = ((bishop_blockers[from] & board.occupied) * BISHOP_MAGICS[from]) >> (NUM_SQUARES - BISHOP_RELEVANT_SQUARES[from]);
        Bitboard attacks = rook_attacks[from][rook_magic_index] | bishop_attacks[from][bishop_magic_index];
        Bitboard own_pieces = white_to_move ? board.white_pieces : board.black_pieces;
        attacks &= ~own_pieces;
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            // focus only on captures for interesting_only
            if (interesting_only && !board.isSquareOccupied(to))
                continue;
            moves.append(Move{from, to, piece});
            // log(QUEEN_MOVE, "Found queen move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

bool MoveGenerator::squareUnderAttack(Board &board, Position square, bool white_is_attacker)
{
    // pretend that the piece on this square could move like a knight/bishop/...
    // could it attack an enemy piece of the corresponding type?
    // if yes, then that enemy piece could also attack this square

    // knight moves
    Bitboard knights = white_is_attacker ? board.white_knights : board.black_knights;
    if (knight_moves[square] & knights)
        return true;

    // diagonal bishop/queen moves
    Bitboard diagonal = white_is_attacker ? (board.white_bishops | board.white_queens) : (board.black_bishops | board.black_queens);
    int bishop_magic_index = ((bishop_blockers[square] & board.occupied) * BISHOP_MAGICS[square]) >> (NUM_SQUARES - BISHOP_RELEVANT_SQUARES[square]);
    if (bishop_attacks[square][bishop_magic_index] & diagonal)
        return true;

    // orthogonal rook/queen moves
    Bitboard orthogonal = white_is_attacker ? (board.white_rooks | board.white_queens) : (board.black_rooks | board.black_queens);
    int rook_magic_index = ((rook_blockers[square] & board.occupied) * ROOK_MAGICS[square]) >> (NUM_SQUARES - ROOK_RELEVANT_SQUARES[square]);
    if (rook_attacks[square][rook_magic_index] & orthogonal)
        return true;

    // king moves
    Bitboard king = white_is_attacker ? board.white_king : board.black_king;
    if (king_moves[square] & king)
        return true;

    // diagonal pawn attacks
    if (white_is_attacker)
        return black_pawn_attack_right[square] & board.white_pawns || black_pawn_attack_left[square] & board.white_pawns;
    return white_pawn_attack_right[square] & board.black_pawns || white_pawn_attack_left[square] & board.black_pawns;
}

bool MoveGenerator::squaresUnderAttack(Board &board, Bitboard squares, bool white_is_attacker)
{
    while (squares)
        if (squareUnderAttack(board, clearRightmostSetBit(squares), white_is_attacker))
            return true;
    return false;
}

Move MoveGenerator::pickBestMove(MoveList &moves)
{
    int best_move_index = 0;
    Score best_score = moves.scores[0];

    for (int i = 1; i < moves.size; i++)
    {
        Score score = moves.scores[i];
        if (score == USED_MOVE)
            continue;
        if (score > best_score)
        {
            best_score = score;
            best_move_index = i;
        }
    }

    moves.scores[best_move_index] = USED_MOVE;
    return moves.moves[best_move_index];
}

bool MoveGenerator::markMoveAsUsed(MoveList &moves, Move move)
{
    if (move == NULL_MOVE)
        return false;

    for (int i = 0; i < moves.size; i++)
        if (moves.moves[i] == move)
        {
            moves.scores[i] = USED_MOVE;
            return true;
        }
    return false;
}

MoveGenerator &MoveGenerator::getInstance()
{
    static MoveGenerator instance;
    return instance;
}

UndoInfo MoveGenerator::makeMove(Board &board, Move &move)
{
    // store information needed for unmake before actually making the move
    UndoInfo undo;
    undo.captured_piece = board.getPieceAt(move.to);
    undo.en_passant = board.en_passant;
    undo.castling_rights = board.castling_rights;
    undo.half_move_clock = board.half_move_clock;

    // capture piece
    if (undo.captured_piece != EMPTY)
    {
        board.half_move_clock = HALF_MOVE_CLOCK_RESET;
        clear(*board.getBitboardByPiece(undo.captured_piece), move.to);
        clear(board.white_to_move ? board.black_pieces : board.white_pieces, move.to);
        clear(board.occupied, move.to);
    }

    // handle en passant
    if (move.piece == WHITE_PAWN || move.piece == BLACK_PAWN)
    {
        board.half_move_clock = HALF_MOVE_CLOCK_RESET;
        if (move.to == board.en_passant)
        {
            Position captured_piece_position = move.to;
            if (board.white_to_move)
            {
                undo.captured_piece = BLACK_PAWN;
                captured_piece_position += DOWN;
                clear(board.black_pawns, captured_piece_position);
                clear(board.black_pieces, captured_piece_position);
            }
            else
            {
                undo.captured_piece = WHITE_PAWN;
                captured_piece_position += UP;
                clear(board.white_pawns, captured_piece_position);
                clear(board.white_pieces, captured_piece_position);
            }
            clear(board.occupied, captured_piece_position);
        }
        detectDoublePawnPushForEnPassant(board, move);
    }
    else
        board.en_passant = NO_EN_PASSANT;

    // move piece <=> update moved piece, own pieces and occupied squares
    Bitboard *piece = board.getBitboardByPiece(move.piece);
    BitBoard::movePiece(*piece, move.from, move.to);
    BitBoard::movePiece(board.white_to_move ? board.white_pieces : board.black_pieces, move.from, move.to);
    BitBoard::movePiece(board.occupied, move.from, move.to);

    handleCastling(board, move);
    if (move.promotion)
    {
        BitBoard::clear(*piece, move.to);
        BitBoard::set(*board.getBitboardByPiece(move.promotion), move.to);
    }
    board.white_to_move = !board.white_to_move;
    board.half_move_clock++;
    return undo;
}

void MoveGenerator::unmakeMove(Board &board, Move move, UndoInfo undo)
{
    board.white_to_move = !board.white_to_move;
    board.half_move_clock = undo.half_move_clock;
    board.castling_rights = undo.castling_rights;
    board.en_passant = undo.en_passant;

    Bitboard *piece = board.getBitboardByPiece(move.piece);
    Bitboard *own_pieces = board.white_to_move ? &board.white_pieces : &board.black_pieces;

    // replace promoted piece with pawn
    if (move.promotion)
    {
        Bitboard *promoted_piece = board.getBitboardByPiece(move.promotion);
        BitBoard::clear(*promoted_piece, move.to);
        BitBoard::set(*piece, move.to);
    }

    // move piece
    BitBoard::movePiece(*piece, move.to, move.from);
    BitBoard::movePiece(*own_pieces, move.to, move.from);
    BitBoard::movePiece(board.occupied, move.to, move.from);

    // place captured piece at its original position (to or en_passant)
    if (undo.captured_piece != EMPTY)
    {
        Bitboard *captured_piece = board.getBitboardByPiece(undo.captured_piece);
        Bitboard *enemy_pieces = board.white_to_move ? &board.black_pieces : &board.white_pieces;
        Position position = move.to;
        bool en_passant_move = (move.to == board.en_passant) && (move.piece == (board.white_to_move ? WHITE_PAWN : BLACK_PAWN));
        if (en_passant_move)
            position += (board.white_to_move ? DOWN : UP);
        BitBoard::set(*captured_piece, position);
        BitBoard::set(*enemy_pieces, position);
        BitBoard::set(board.occupied, position);
    }

    // place rook where it was before castling
    if (move.castling)
    {
        Bitboard *rooks = board.getBitboardByPiece(board.white_to_move ? WHITE_ROOK : BLACK_ROOK);
        bool king_side_castling = move.castling & (board.white_to_move ? WHITE_KING_SIDE_CASTLING : BLACK_KING_SIDE_CASTLING);
        move.from += king_side_castling ? RIGHT : LEFT;
        move.to += king_side_castling ? RIGHT : 2 * LEFT;
        BitBoard::movePiece(*rooks, move.from, move.to);
        BitBoard::movePiece(*own_pieces, move.from, move.to);
        BitBoard::movePiece(board.occupied, move.from, move.to);
    }
}

void MoveGenerator::handleCastling(Board &board, Move &move)
{
    if (!board.castling_rights)
        return;

    // todo simplify constant fetching, maybe two constant classes one for white and one for black
    Bitboard king_side = board.white_to_move ? WHITE_KING_SIDE_CASTLING : BLACK_KING_SIDE_CASTLING;
    Bitboard queen_side = board.white_to_move ? WHITE_QUEEN_SIDE_CASTLING : BLACK_QUEEN_SIDE_CASTLING;
    Bitboard *own_rooks = board.white_to_move ? &board.white_rooks : &board.black_rooks;
    Bitboard *own_pieces = board.white_to_move ? &board.white_pieces : &board.black_pieces;
    Bitboard *occupied = &board.occupied;

    // apply castling move
    if (move.castling)
    {
        bool king_side_castling = move.castling & king_side;
        bool queen_side_castling = move.castling & queen_side;
        assert(king_side_castling != queen_side_castling, "Castling move must be either queen side or king side.");
        board.castling_rights &= ~queen_side & ~king_side; // clear castling rights

        Position rook_from = move.from;
        Position rook_to = move.from;
        rook_from += queen_side_castling ? 4 * LEFT : 3 * RIGHT;
        rook_to += queen_side_castling ? LEFT : RIGHT;

        // update rook position, king already updated by makeMove
        BitBoard::movePiece(*own_rooks, rook_from, rook_to);
        BitBoard::movePiece(*own_pieces, rook_from, rook_to);
        BitBoard::movePiece(*occupied, rook_from, rook_to);
        return;
    }

    // update castling rights if own king or rook moved or enemy rook captured
    Bitboard king_side_rook_start_position = board.white_to_move ? WHITE_KING_SIDE_ROOK_START_POSITION : BLACK_KING_SIDE_ROOK_START_POSITION;
    Bitboard queen_side_rook_start_position = board.white_to_move ? WHITE_QUEEN_SIDE_ROOK_START_POSITION : BLACK_QUEEN_SIDE_ROOK_START_POSITION;
    Bitboard enemy_king_side_rook_start_position = board.white_to_move ? BLACK_KING_SIDE_ROOK_START_POSITION : WHITE_KING_SIDE_ROOK_START_POSITION;
    Bitboard enemy_queen_side_rook_start_position = board.white_to_move ? BLACK_QUEEN_SIDE_ROOK_START_POSITION : WHITE_QUEEN_SIDE_ROOK_START_POSITION;

    bool king_moved = move.piece == (board.white_to_move ? WHITE_KING : BLACK_KING);
    bool own_king_side_rook_moved = !(*own_rooks & king_side_rook_start_position);
    bool own_queen_side_rook_moved = !(*own_rooks & queen_side_rook_start_position);
    bool enemy_queen_side_rook_captured = *own_pieces & enemy_queen_side_rook_start_position;
    bool enemy_king_side_rook_captured = *own_pieces & enemy_king_side_rook_start_position;
    if (king_moved)
        board.castling_rights &= ~king_side & ~queen_side;
    if (own_king_side_rook_moved)
        board.castling_rights &= ~king_side;
    if (own_queen_side_rook_moved)
        board.castling_rights &= ~queen_side;
    if (enemy_queen_side_rook_captured)
        board.castling_rights &= ~(board.white_to_move ? BLACK_QUEEN_SIDE_CASTLING : WHITE_QUEEN_SIDE_CASTLING);
    if (enemy_king_side_rook_captured)
        board.castling_rights &= ~(board.white_to_move ? BLACK_KING_SIDE_CASTLING : WHITE_KING_SIDE_CASTLING);
}

void MoveGenerator::detectDoublePawnPushForEnPassant(Board &board, Move &move)
{
    Position from_row = move.from / 8;
    Position to_row = move.to / 8;
    board.en_passant = NO_EN_PASSANT;
    // mark en passant square if a pawn moved two squares forward and an enemy pawn can capture it
    if (board.white_to_move && (from_row == ROW_2 && to_row == ROW_4))
    {
        Position ep_square = move.from + UP;
        if ((white_pawn_attack_left[ep_square] | white_pawn_attack_right[ep_square]) & board.black_pawns)
            board.en_passant = ep_square;
    }
    else if (!board.white_to_move && (from_row == ROW_7 && to_row == ROW_5))
    {
        Position ep_square = move.from + DOWN;
        if ((black_pawn_attack_left[ep_square] | black_pawn_attack_right[ep_square]) & board.white_pawns)
            board.en_passant = ep_square;
    }
}
