#pragma once
#include "utils.h"
#include "Board.h"
#include <vector>

struct Move
{
    Position from;
    Position to;
    Promotion promotion = NO_PROMOTION;

    Move(Position from = 0, Position to = 0, Promotion promotion = NO_PROMOTION)
        : from(from), to(to), promotion(promotion) {}
    Move(Move16 encoded_move);

    bool isUndo() const;
    bool isCastlingCandidate() const;
    bool isPromotion() const;
    Piece getPromotionPiece(bool white_to_move);

    std::string toString() const;
    bool operator==(const Move &rhs);
    bool operator!=(const Move &rhs);
};

const Move UNDO_MOVE = Move{0, 0, UNDO};
const Move NULL_MOVE = Move{0, 0, NO_PROMOTION};

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
    HalfMoveClock half_move_clock;
};

struct UndoHistory
{
    UndoInfo undo_infos[MAX_SEARCH_DEPTH + MAX_QUIESCENCE_DEPTH];
    int size = 0;

    void push(UndoInfo undo);
    UndoInfo pop();
    void clear();
};