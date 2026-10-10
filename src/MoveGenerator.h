#pragma once
#include "definitions.h"
#include "BitBoard.h"
#include "Logger.h"
#include "Magics.h"
#include "utils.h"
#include <vector>

using namespace BitBoard;

struct Move
{
    Position from;
    Position to;
    Piece piece;
    PromotionAndCastlingRights promotion_and_castling = NO_PROMOTION_OR_CASTLING;

    bool operator==(const Move &rhs)
    {
        return from == rhs.from && to == rhs.to && piece == rhs.piece && promotion_and_castling == rhs.promotion_and_castling;
    }

    bool operator!=(const Move &rhs)
    {
        return from != rhs.from || to != rhs.to || piece != rhs.piece || promotion_and_castling != rhs.promotion_and_castling;
    }

    std::string toString() const
    {
        std::string result;
        result += getSquareName(from);
        result += getSquareName(to);
        if (promotion_and_castling & PROMOTE_TO_QUEEN)
            result += 'q';
        if (promotion_and_castling & PROMOTE_TO_ROOK)
            result += 'r';
        if (promotion_and_castling & PROMOTE_TO_BISHOP)
            result += 'b';
        if (promotion_and_castling & PROMOTE_TO_KNIGHT)
            result += 'n';
        if (promotion_and_castling & (WHITE_KING_CASTLING | BLACK_KING_CASTLING))
            result += "O-O";
        if (promotion_and_castling & (WHITE_QUEEN_CASTLING | BLACK_QUEEN_CASTLING))
            result += "O-O-O";
        return result;
    }

    bool isPromotion() const
    {
        return promotion_and_castling & (PROMOTE_TO_QUEEN | PROMOTE_TO_ROOK | PROMOTE_TO_BISHOP | PROMOTE_TO_KNIGHT);
    }

    bool isCastling() const
    {
        return promotion_and_castling & (WHITE_KING_CASTLING | WHITE_QUEEN_CASTLING | BLACK_KING_CASTLING | BLACK_QUEEN_CASTLING);
    }
};

struct UndoInfo
{
    Piece captured_piece;
    Position en_passant;
    CastlingRights castling_rights;
    Clock half_move_clock;
};

struct UndoHistory
{
    UndoInfo undo_info[MAX_SEARCH_DEPTH + MAX_QUIESCENCE_DEPTH];
    int size = 0;

    void push(UndoInfo undo)
    {
        undo_info[size++] = undo;
    }

    UndoInfo pop()
    {
        return undo_info[--size];
    }
};

const Move UNDO_MOVE = Move{0, 0, UNDO, NO_PROMOTION_OR_CASTLING};
const Move NULL_MOVE = Move{0, 0, EMPTY, NO_PROMOTION_OR_CASTLING};

struct MoveList
{
    Move moves[MAX_MOVES];
    Score scores[MAX_MOVES];
    int size = 0;

    void append(Move move)
    {
        moves[size++] = move;
    }

    void clear()
    {
        size = 0;
    }

    bool empty()
    {
        return size == 0;
    }
};

class MoveGenerator
{
private:
    MoveGenerator();
    static MoveGenerator instance;

    // move generation masks
    void initializePawnCaptureMasks();
    void initializeKnightMoves();
    void initializeBishopBlockers();
    void initializeRookBlockers();
    void initializeKingMoves();

    // move generation
    void addPawnMoveWithPossiblePromotion(MoveList &moves, Move move, bool white_to_move);
    void addCastlingMoves(const Board &board, MoveList &moves);
    void generateKingMoves(const Board &board, MoveList &moves, bool interesting_only = false);
    void generatePawnMoves(const Board &board, MoveList &moves, bool interesting_only = false);
    void generateKnightMoves(const Board &board, MoveList &moves, bool interesting_only = false);
    void generateBishopMoves(const Board &board, MoveList &moves, bool interesting_only = false);
    void generateRookMoves(const Board &board, MoveList &moves, bool interesting_only = false);
    void generateQueenMoves(const Board &board, MoveList &moves, bool interesting_only = false);

    // precomputed attacks for sliding pieces (bishop and rook) using magic bitboards
    void initializeRookBishopAttacks();                                           // precomputes squares under attack for every position for every relevant occupancy
    Bitboard getOccupancyVariation(int index, int relevant_bits, Bitboard moves); // returns index-th occupancy variation of moves
    Bitboard precomputeSlidingRookAttacks(Position square, Bitboard occupied);
    Bitboard precomputeSlidingBishopAttacks(Position square, Bitboard occupied);

    // move application
    void handleCastling(Board &board, Move move);
    void detectDoublePawnPushForEnPassant(Board &board, Move move);

public:
    // attack masks
    Bitboard white_pawn_attack_right[NUM_SQUARES];
    Bitboard white_pawn_attack_left[NUM_SQUARES];
    Bitboard black_pawn_attack_right[NUM_SQUARES];
    Bitboard black_pawn_attack_left[NUM_SQUARES];
    Bitboard knight_moves[NUM_SQUARES];
    Bitboard queen_moves[NUM_SQUARES];
    Bitboard king_moves[NUM_SQUARES];

    // precomputed attacks for sliding pieces
    Bitboard bishop_blockers[NUM_SQUARES];
    Bitboard rook_blockers[NUM_SQUARES];
    Bitboard bishop_attacks[NUM_SQUARES][512];
    Bitboard rook_attacks[NUM_SQUARES][4096];

    static MoveGenerator &getInstance();
    UndoInfo makeMove(Board &board, Move move);
    void unmakeMove(Board &board, Move move, UndoInfo undo);
    MoveList generateLegalMoves(Board &board, bool interesting_only = false);
    MoveList generatePseudoLegalMoves(Board &board, bool interesting_only = false);
    bool isKingSafe(const Board &board, bool king_is_white);
    bool squareUnderAttack(const Board &board, Position square, bool white_is_attacker);
    bool squaresUnderAttack(const Board &board, Bitboard squares, bool white_is_attacker);
    Move pickBestMove(MoveList &moves);
    bool markMoveAsUsed(MoveList &moves, Move move);
};
