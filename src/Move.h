#pragma once
#include "utils.h"
#include <vector>

struct Move
{
    Position from;
    Position to;
    Piece piece;
    PromotionAndCastlingRights promotion_and_castling = NO_PROMOTION_OR_CASTLING;

    bool operator==(const Move &rhs);
    bool operator!=(const Move &rhs);

    bool isPromotion() const;
    bool isCastling() const;
    bool isKingSideCastling() const;
    bool isQueenSideCastling() const;

    std::string toString() const;
};

const Move UNDO_MOVE = Move{0, 0, UNDO, NO_PROMOTION_OR_CASTLING};
const Move NULL_MOVE = Move{0, 0, EMPTY, NO_PROMOTION_OR_CASTLING};

struct MoveList
{
    Move moves[MAX_MOVES];
    Score scores[MAX_MOVES];
    int size = 0;

    void append(Move move);
    void clear();
    bool empty();
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

    void push(UndoInfo undo);
    UndoInfo pop();
    void clear();
};