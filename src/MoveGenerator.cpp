#include "MoveGenerator.h"

MoveGenerator::MoveGenerator()
{
    initializePawnCaptureMasks();
    initializeKnightMoves();
    initializeBishopBlockers();
    initializeRookBlockers();
    initializeKingMoves();
    initializeRookBishopAttacks();
    initializeCastlingRightsUpdate();
}

void MoveGenerator::initializePawnCaptureMasks()
{
    log(MOVE_GENERATOR, "Initializing pawn capture masks.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = getRow(square);
        Position col = getCol(square);

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
        Position row = getRow(square);
        Position col = getCol(square);

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
        Position row = getRow(square);
        Position col = getCol(square);

        if (row < ROW_8)
        {
            if (col < COL_H)
                for (Position i = square + UP_RIGHT; (i < NUM_SQUARES + DOWN && getCol(i) != COL_H); i += UP_RIGHT)
                    set(bishop_blockers[square], i);

            if (col > COL_A)
                for (Position i = square + UP_LEFT; (i < NUM_SQUARES + DOWN && getCol(i) != COL_A); i += UP_LEFT)
                    set(bishop_blockers[square], i);
        }

        if (row > ROW_1)
        {
            if (col < COL_H)
                for (Position i = square + DOWN_RIGHT; (i >= UP && getCol(i) != COL_H); i += DOWN_RIGHT)
                    set(bishop_blockers[square], i);

            if (col > COL_A)
                for (Position i = square + DOWN_LEFT; (i >= UP && getCol(i) != COL_A); i += DOWN_LEFT)
                    set(bishop_blockers[square], i);
        }
    }
}

void MoveGenerator::initializeRookBlockers()
{
    log(MOVE_GENERATOR, "Initializing rook blockers.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = getRow(square);
        Position col = getCol(square);

        if (col < COL_H)
            for (Position i = square + RIGHT; getCol(i) != COL_H; i += RIGHT)
                set(rook_blockers[square], i);

        if (col > COL_A)
            for (Position i = square + LEFT; getCol(i) != COL_A; i += LEFT)
                set(rook_blockers[square], i);

        for (Position i = square + UP; i < NUM_SQUARES + DOWN; i += UP)
            set(rook_blockers[square], i);

        for (Position i = square + DOWN; i >= UP; i += DOWN)
            set(rook_blockers[square], i);
    }
}

void MoveGenerator::initializeKingMoves()
{
    log(MOVE_GENERATOR, "Initializing king moves.");
    for (Position square = 0; square < NUM_SQUARES; square++)
    {
        Position row = getRow(square);
        Position col = getCol(square);

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

void MoveGenerator::addPawnMoveWithPossiblePromotion(MoveList &moves, Move move)
{
    // regular move
    if (move.to >= UP && move.to < NUM_SQUARES + DOWN)
    {
        moves.append(move);
        // log(PAWN_MOVE, "Found pawn move from " + getSquareName(move.from) + " to " + getSquareName(move.to) + ".");
        return;
    }

    // pawn promotion
    move.promotion = PROMOTE_TO_QUEEN;
    moves.append(move);
    move.promotion = PROMOTE_TO_ROOK;
    moves.append(move);
    move.promotion = PROMOTE_TO_BISHOP;
    moves.append(move);
    move.promotion = PROMOTE_TO_KNIGHT;
    moves.append(move);
    // log(PAWN_MOVE, "Found pawn promotion from " + getSquareName(move.from) + " to " + getSquareName(move.to) + ".");
}

void MoveGenerator::addCastlingMoves(const Board &board, MoveList &moves)
{
    bool white_to_move = board.white_to_move;
    Position king_start = white_to_move ? WHITE_KING_START_POSITION : BLACK_KING_START_POSITION;

    // king side castling: king moves two squares to the right, rook moves to the left of the king
    CastlingRights king_side_castling = white_to_move ? WHITE_KING_CASTLING : BLACK_KING_CASTLING;
    if (board.castling_rights & king_side_castling)
    {
        Bitboard free_king_squares = white_to_move ? WHITE_KING_CASTLING_FREE_SQUARES : BLACK_KING_CASTLING_FREE_SQUARES;
        Bitboard king_side_blocked = board.occupied & free_king_squares;
        if (!king_side_blocked) // all squares between king and rook must be empty
        {
            if (!areSquaresUnderAttack(board, free_king_squares, !white_to_move)) // squares that the king passes through must not be attacked
            {
                Position to = king_start + RIGHT * 2;
                moves.append(Move{king_start, to});
                // log(KING_MOVE, "Found king side castling from " + getSquareName(king_start) + " to " + getSquareName(to) + ".");
            }
        }
    }

    // queen side castling: king moves two squares to the left, rook moves to the right of the king
    CastlingRights queen_side_castling = white_to_move ? WHITE_QUEEN_CASTLING : BLACK_QUEEN_CASTLING;
    if (board.castling_rights & queen_side_castling)
    {
        Bitboard free_queen_squares = white_to_move ? WHITE_QUEEN_CASTLING_FREE_SQUARES : BLACK_QUEEN_CASTLING_FREE_SQUARES;
        Bitboard queen_side_blocked = board.occupied & free_queen_squares;
        if (!queen_side_blocked) // all squares between king and rook must be empty
        {
            Bitboard safe_squares = white_to_move ? WHITE_QUEEN_CASTLING_SAFE_SQUARES : BLACK_QUEEN_CASTLING_SAFE_SQUARES;
            if (!areSquaresUnderAttack(board, safe_squares, !white_to_move)) // squares that the king passes through must not be attacked
            {
                Position to = king_start + LEFT * 2;
                moves.append(Move{king_start, to});
                // log(KING_MOVE, "Found queen side castling from " + getSquareName(king_start) + " to " + getSquareName(to) + ".");
            }
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

void MoveGenerator::initializeCastlingRightsUpdate()
{
    for (Position square = 0; square < NUM_SQUARES; square++)
        castling_rights_update[square] = ALL_CASTLING;

    castling_rights_update[WHITE_KING_START_POSITION] &= ~(WHITE_CASTLING);
    castling_rights_update[BLACK_KING_START_POSITION] &= ~(BLACK_CASTLING);
    castling_rights_update[WHITE_QUEEN_ROOK_START_POSITION_NEW] &= ~(WHITE_QUEEN_CASTLING);
    castling_rights_update[WHITE_KING_ROOK_START_POSITION_NEW] &= ~(WHITE_KING_CASTLING);
    castling_rights_update[BLACK_QUEEN_ROOK_START_POSITION_NEW] &= ~(BLACK_QUEEN_CASTLING);
    castling_rights_update[BLACK_KING_ROOK_START_POSITION_NEW] &= ~(BLACK_KING_CASTLING);
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
    for (Position i = square + RIGHT; getCol(i) != COL_A; i += RIGHT)
    {
        set(attacks, i);
        if (isSet(occupied, i))
            break;
    }

    // need 1 >= 0 since modulo of negative numbers are negative in C
    for (Position i = square + LEFT; i >= 0 && getCol(i) != COL_H; i += LEFT)
    {
        set(attacks, i);
        if (isSet(occupied, i))
            break;
    }

    for (Position i = square + UP; i < NUM_SQUARES; i += UP)
    {
        set(attacks, i);
        if (isSet(occupied, i))
            break;
    }

    for (Position i = square + DOWN; i >= ROW_1; i += DOWN)
    {
        set(attacks, i);
        if (isSet(occupied, i))
            break;
    }

    return attacks;
}

Bitboard MoveGenerator::precomputeSlidingBishopAttacks(Position square, Bitboard occupied)
{
    Bitboard attacks = 0;
    for (Position i = square + UP_RIGHT; (i < NUM_SQUARES && getCol(i) != COL_A); i += UP_RIGHT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    for (Position i = square + UP_LEFT; (i < NUM_SQUARES && getCol(i) != COL_H); i += UP_LEFT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    for (Position i = square + DOWN_RIGHT; (i >= ROW_1 && getCol(i) != COL_A); i += DOWN_RIGHT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    for (Position i = square + DOWN_LEFT; (i >= ROW_1 && getCol(i) != COL_H); i += DOWN_LEFT)
    {
        attacks |= (1ULL << i);
        if (occupied & (1ULL << i))
            break;
    }

    return attacks;
}

MoveList MoveGenerator::generateLegalMoves(Board &board, bool interesting_only)
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

bool MoveGenerator::isKingSafe(const Board &board, bool king_is_white)
{
    Position king_square = getRightmostSetBit(board.piece_bitboards[king_is_white ? WHITE_KING : BLACK_KING]);
    return !isSquareUnderAttack(board, king_square, !king_is_white);
}

void MoveGenerator::generateKingMoves(const Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Bitboard king = board.piece_bitboards[white_to_move ? WHITE_KING : BLACK_KING];
    Bitboard own_pieces = board.colors[white_to_move];

    // regular moves and attacks
    Position from = getRightmostSetBit(king);
    Bitboard attacks = king_moves[from] & ~own_pieces;
    if (interesting_only)
        attacks &= board.occupied; // only consider captures
    while (attacks)
    {
        Position to = clearRightmostSetBit(attacks);
        // move may not put king into check
        if (isSquareUnderAttack(board, to, !white_to_move))
            continue;
        moves.append(Move{from, to});
        // log(KING_MOVE, "Found king move from " + getSquareName(from) + " to " + getSquareName(to) + ".");
    }

    // castling moves
    if (!interesting_only && !isSquareUnderAttack(board, from, !white_to_move))
        addCastlingMoves(board, moves);
}

void MoveGenerator::generatePawnMoves(const Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Bitboard pawns = board.piece_bitboards[white_to_move ? WHITE_PAWN : BLACK_PAWN];
    Direction direction = white_to_move ? UP : DOWN;

    // right shift for white, left shift for black since negative shift is undefined
    Bitboard single_push = white_to_move ? (pawns << UP) : (pawns >> UP);
    single_push &= ~board.occupied;
    if (interesting_only)
        single_push &= PROMOTION_ROWS; // only consider promotions
    else
    {
        Bitboard double_push = white_to_move ? ((single_push & WHITE_DOUBLE_PUSH_ROW) << 8) : ((single_push & BLACK_DOUBLE_PUSH_ROW) >> 8);
        double_push &= ~board.occupied;
        while (double_push)
        {
            Position to = clearRightmostSetBit(double_push);
            Position from = to - 2 * direction;
            moves.append(Move{from, to});
        }
    }

    while (single_push)
    {
        Position to = clearRightmostSetBit(single_push);
        Position from = to - direction;
        addPawnMoveWithPossiblePromotion(moves, Move{from, to});
    }

    // pawn attacks
    Bitboard enemies = board.colors[!white_to_move];
    if (board.en_passant != NO_EN_PASSANT)
        set(enemies, board.en_passant); // set en passant square as capturable
    Direction direction_right = white_to_move ? UP_RIGHT : DOWN_RIGHT;
    Direction direction_left = white_to_move ? UP_LEFT : DOWN_LEFT;
    Bitboard attacks_right = pawns & ~RIGHT_COL;
    attacks_right = (white_to_move ? (attacks_right << UP_RIGHT) : (attacks_right >> UP_LEFT));
    attacks_right &= enemies;
    while (attacks_right)
    {
        Position to = clearRightmostSetBit(attacks_right);
        Position from = to - direction_right;
        addPawnMoveWithPossiblePromotion(moves, Move{from, to});
    }

    Bitboard attacks_left = pawns & ~LEFT_COL;
    attacks_left = (white_to_move ? (attacks_left << UP_LEFT) : (attacks_left >> UP_RIGHT));
    attacks_left &= enemies;
    while (attacks_left)
    {
        Position to = clearRightmostSetBit(attacks_left);
        Position from = to - direction_left;
        addPawnMoveWithPossiblePromotion(moves, Move{from, to});
    }
}

void MoveGenerator::generateKnightMoves(const Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Bitboard knights = board.piece_bitboards[white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT];
    Bitboard own_pieces = board.colors[white_to_move];

    while (knights)
    {
        Position from = clearRightmostSetBit(knights);
        Bitboard attacks = knight_moves[from] & ~own_pieces;
        if (interesting_only)
            attacks &= board.occupied; // only consider captures
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            moves.append(Move{from, to});
            // log(KNIGHT_MOVE, "Found knight move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

void MoveGenerator::generateBishopMoves(const Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Bitboard bishops = board.piece_bitboards[white_to_move ? WHITE_BISHOP : BLACK_BISHOP];
    Bitboard own_pieces = board.colors[white_to_move];

    // for each bishop add each attacked square to moves
    while (bishops)
    {
        Position from = clearRightmostSetBit(bishops);
        // i would love to write an insightful comment here, but i have no idea how magic bitboards work
        // after all, they are by definition magic ¯\_(ツ)_/¯
        int magic_index = ((bishop_blockers[from] & board.occupied) * BISHOP_MAGICS[from]) >> (NUM_SQUARES - BISHOP_RELEVANT_SQUARES[from]);
        Bitboard attacks = bishop_attacks[from][magic_index] & ~own_pieces;
        if (interesting_only)
            attacks &= board.occupied; // only consider captures
        // add each square attacked by current bishop to moves
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            moves.append(Move{from, to});
            // log(BISHOP_MOVE, "Found bishop move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

void MoveGenerator::generateRookMoves(const Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;
    Bitboard rooks = board.piece_bitboards[white_to_move ? WHITE_ROOK : BLACK_ROOK];
    Bitboard own_pieces = board.colors[white_to_move];

    // for each rook add each attacked square to moves
    while (rooks)
    {
        Position from = clearRightmostSetBit(rooks);
        int magic_index = ((rook_blockers[from] & board.occupied) * ROOK_MAGICS[from]) >> (NUM_SQUARES - ROOK_RELEVANT_SQUARES[from]);
        Bitboard attacks = rook_attacks[from][magic_index] & ~own_pieces;
        if (interesting_only)
            attacks &= board.occupied; // only consider captures
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            moves.append(Move{from, to});
            // log(ROOK_MOVE, "Found rook move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

void MoveGenerator::generateQueenMoves(const Board &board, MoveList &moves, bool interesting_only)
{
    bool white_to_move = board.white_to_move;

    // for each queen add each attacked square to moves
    Bitboard queens = board.piece_bitboards[white_to_move ? WHITE_QUEEN : BLACK_QUEEN];
    while (queens)
    {
        Position from = clearRightmostSetBit(queens);
        int rook_magic_index = ((rook_blockers[from] & board.occupied) * ROOK_MAGICS[from]) >> (NUM_SQUARES - ROOK_RELEVANT_SQUARES[from]);
        int bishop_magic_index = ((bishop_blockers[from] & board.occupied) * BISHOP_MAGICS[from]) >> (NUM_SQUARES - BISHOP_RELEVANT_SQUARES[from]);
        Bitboard attacks = rook_attacks[from][rook_magic_index] | bishop_attacks[from][bishop_magic_index];
        Bitboard own_pieces = board.colors[white_to_move];
        attacks &= ~own_pieces;
        if (interesting_only)
            attacks &= board.occupied; // only consider captures
        while (attacks)
        {
            Position to = clearRightmostSetBit(attacks);
            moves.append(Move{from, to});
            // log(QUEEN_MOVE, "Found queen move from " + getSquareName(square) + " to " + getSquareName(to) + ".");
        }
    }
}

bool MoveGenerator::isSquareUnderAttack(const Board &board, Position square, bool is_white_attacking)
{
    // pretend that the piece on this square could move like a knight/bishop/...
    // could it attack an enemy piece of the corresponding type?
    // if yes, then that enemy piece could also attack this square

    // knight moves
    Bitboard knights = board.piece_bitboards[is_white_attacking ? WHITE_KNIGHT : BLACK_KNIGHT];
    if (knight_moves[square] & knights)
        return true;

    // diagonal bishop/queen moves
    Bitboard diagonal = is_white_attacking ? (board.piece_bitboards[WHITE_BISHOP] | board.piece_bitboards[WHITE_QUEEN]) : (board.piece_bitboards[BLACK_BISHOP] | board.piece_bitboards[BLACK_QUEEN]);
    int bishop_magic_index = ((bishop_blockers[square] & board.occupied) * BISHOP_MAGICS[square]) >> (NUM_SQUARES - BISHOP_RELEVANT_SQUARES[square]);
    if (bishop_attacks[square][bishop_magic_index] & diagonal)
        return true;

    // orthogonal rook/queen moves
    Bitboard orthogonal = is_white_attacking ? (board.piece_bitboards[WHITE_ROOK] | board.piece_bitboards[WHITE_QUEEN]) : (board.piece_bitboards[BLACK_ROOK] | board.piece_bitboards[BLACK_QUEEN]);
    int rook_magic_index = ((rook_blockers[square] & board.occupied) * ROOK_MAGICS[square]) >> (NUM_SQUARES - ROOK_RELEVANT_SQUARES[square]);
    if (rook_attacks[square][rook_magic_index] & orthogonal)
        return true;

    // king moves
    Bitboard king = board.piece_bitboards[is_white_attacking ? WHITE_KING : BLACK_KING];
    if (king_moves[square] & king)
        return true;

    // diagonal pawn attacks
    if (is_white_attacking)
        return black_pawn_attack_right[square] & board.piece_bitboards[WHITE_PAWN] || black_pawn_attack_left[square] & board.piece_bitboards[WHITE_PAWN];
    return white_pawn_attack_right[square] & board.piece_bitboards[BLACK_PAWN] || white_pawn_attack_left[square] & board.piece_bitboards[BLACK_PAWN];
}

bool MoveGenerator::areSquaresUnderAttack(const Board &board, Bitboard squares, bool is_white_attacking)
{
    while (squares)
        if (isSquareUnderAttack(board, clearRightmostSetBit(squares), is_white_attacking))
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

UndoInfo MoveGenerator::makeMove(Board &board, Move move)
{
    // store information needed for unmake before actually making the move
    UndoInfo undo;
    undo.captured_piece = board.pieces[move.to];
    undo.en_passant = board.en_passant;
    undo.castling_rights = board.castling_rights;
    undo.half_move_clock = board.half_move_clock;

    // capture piece
    if (undo.captured_piece != EMPTY)
    {
        board.half_move_clock = HALF_MOVE_CLOCK_RESET;
        clear(board.piece_bitboards[undo.captured_piece], move.to);
        clear(board.colors[!board.white_to_move], move.to);
        clear(board.occupied, move.to);
    }

    // handle en passant
    Piece moving_piece = board.pieces[move.from];
    if (moving_piece == WHITE_PAWN || moving_piece == BLACK_PAWN)
    {
        board.half_move_clock = HALF_MOVE_CLOCK_RESET;
        if (move.to == board.en_passant)
        {
            Position captured_piece_position = move.to + (board.white_to_move ? DOWN : UP);
            undo.captured_piece = board.white_to_move ? BLACK_PAWN : WHITE_PAWN;
            board.pieces[captured_piece_position] = EMPTY;
            clear(board.piece_bitboards[undo.captured_piece], captured_piece_position);
            clear(board.colors[!board.white_to_move], captured_piece_position);
            clear(board.occupied, captured_piece_position);
        }
        detectDoublePawnPushForEnPassant(board, move);
    }
    else
        board.en_passant = NO_EN_PASSANT;

    // move piece <=> update moved piece, own pieces and occupied squares
    board.pieces[move.to] = moving_piece;
    board.pieces[move.from] = EMPTY;
    movePiece(board.piece_bitboards[moving_piece], move.from, move.to);
    movePiece(board.colors[board.white_to_move], move.from, move.to);
    movePiece(board.occupied, move.from, move.to);

    // update castling rights if king or rook moved/captured
    board.castling_rights &= castling_rights_update[move.from] & castling_rights_update[move.to];

    // additionally move rook if castling
    bool is_king_move = board.pieces[move.to] == (board.white_to_move ? WHITE_KING : BLACK_KING);
    if (move.isCastling(is_king_move))
    {
        Piece own_rook = board.white_to_move ? WHITE_ROOK : BLACK_ROOK;
        bool is_king_side_castling = move.from < move.to;
        move.from += is_king_side_castling ? 3 * RIGHT : 4 * LEFT;
        move.to += is_king_side_castling ? LEFT : RIGHT;
        board.pieces[move.from] = EMPTY;
        board.pieces[move.to] = own_rook;
        movePiece(board.piece_bitboards[own_rook], move.from, move.to);
        movePiece(board.colors[board.white_to_move], move.from, move.to);
        movePiece(board.occupied, move.from, move.to);
    }

    // replace pawn with promoted piece if promotion
    if (move.isPromotion())
    {
        Piece promotion_piece = move.getPromotionPiece(board.white_to_move);
        board.pieces[move.to] = promotion_piece;
        clear(board.piece_bitboards[moving_piece], move.to);
        set(board.piece_bitboards[promotion_piece], move.to);
    }
    board.white_to_move = !board.white_to_move;
    board.half_move_clock++;
    return undo;
}

void MoveGenerator::unmakeMove(Board &board, Move move, UndoInfo undo)
{
    // the player who made the move is now the one to move
    board.white_to_move = !board.white_to_move;
    board.half_move_clock = undo.half_move_clock;
    board.castling_rights = undo.castling_rights;
    board.en_passant = undo.en_passant;

    bool white_to_move = board.white_to_move;
    Piece own_pawn = white_to_move ? WHITE_PAWN : BLACK_PAWN;
    Piece moving_piece = move.isPromotion() ? own_pawn : board.pieces[move.to];

    // replace promoted piece with pawn
    if (move.isPromotion())
    {
        Piece promotion_piece = board.pieces[move.to];
        board.pieces[move.to] = own_pawn;
        clear(board.piece_bitboards[promotion_piece], move.to);
        set(board.piece_bitboards[moving_piece], move.to);
    }

    // move piece
    board.pieces[move.from] = moving_piece;
    board.pieces[move.to] = EMPTY;
    movePiece(board.piece_bitboards[moving_piece], move.to, move.from);
    movePiece(board.colors[white_to_move], move.to, move.from);
    movePiece(board.occupied, move.to, move.from);

    // place captured piece at its original position (to or en_passant)
    if (undo.captured_piece != EMPTY)
    {
        Position position = move.to;
        bool is_en_passant_move = (move.to == board.en_passant) && (moving_piece == own_pawn);
        if (is_en_passant_move)
            position += (white_to_move ? DOWN : UP);
        board.pieces[position] = undo.captured_piece;
        set(board.piece_bitboards[undo.captured_piece], position);
        set(board.colors[!white_to_move], position);
        set(board.occupied, position);
    }

    // place rook where it was before castling
    bool is_king_move = moving_piece == (white_to_move ? WHITE_KING : BLACK_KING);
    if (move.isCastling(is_king_move))
    {
        bool is_king_side_castling = move.from < move.to;
        move.from += is_king_side_castling ? RIGHT : LEFT;
        move.to += is_king_side_castling ? RIGHT : 2 * LEFT;
        board.pieces[move.to] = white_to_move ? WHITE_ROOK : BLACK_ROOK;
        board.pieces[move.from] = EMPTY;
        movePiece(board.piece_bitboards[white_to_move ? WHITE_ROOK : BLACK_ROOK], move.from, move.to);
        movePiece(board.colors[white_to_move], move.from, move.to);
        movePiece(board.occupied, move.from, move.to);
    }
}

void MoveGenerator::detectDoublePawnPushForEnPassant(Board &board, Move move)
{
    Position from_row = getRow(move.from);
    Position to_row = getRow(move.to);
    board.en_passant = NO_EN_PASSANT;
    // mark en passant square if a pawn moved two squares forward and an enemy pawn can capture it
    if (board.white_to_move && (from_row == ROW_2 && to_row == ROW_4))
    {
        Position ep_square = move.from + UP;
        if ((white_pawn_attack_left[ep_square] | white_pawn_attack_right[ep_square]) & board.piece_bitboards[BLACK_PAWN])
            board.en_passant = ep_square;
    }
    else if (!board.white_to_move && (from_row == ROW_7 && to_row == ROW_5))
    {
        Position ep_square = move.from + DOWN;
        if ((black_pawn_attack_left[ep_square] | black_pawn_attack_right[ep_square]) & board.piece_bitboards[WHITE_PAWN])
            board.en_passant = ep_square;
    }
}
