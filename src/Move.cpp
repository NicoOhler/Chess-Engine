#include "Move.h"

bool Move::operator==(const Move &rhs)
{
    return from == rhs.from && to == rhs.to && promotion == rhs.promotion;
}

bool Move::operator!=(const Move &rhs)
{
    return from != rhs.from || to != rhs.to || promotion != rhs.promotion;
}

bool Move::isPromotion() const
{
    return promotion & (PROMOTE_TO_QUEEN | PROMOTE_TO_ROOK | PROMOTE_TO_BISHOP | PROMOTE_TO_KNIGHT);
}

bool Move::isCastlingCandidate() const
{
    if (from == WHITE_KING_START_POSITION && to == WHITE_KING_START_POSITION + 2)
        return true;
    if (from == WHITE_KING_START_POSITION && to == WHITE_KING_START_POSITION - 2)
        return true;
    if (from == BLACK_KING_START_POSITION && to == BLACK_KING_START_POSITION + 2)
        return true;
    if (from == BLACK_KING_START_POSITION && to == BLACK_KING_START_POSITION - 2)
        return true;
    return false;
}

Piece Move::getPromotionPiece(bool white_to_move)
{
    switch (promotion)
    {
    case PROMOTE_TO_QUEEN:
        return white_to_move ? WHITE_QUEEN : BLACK_QUEEN;
    case PROMOTE_TO_ROOK:
        return white_to_move ? WHITE_ROOK : BLACK_ROOK;
    case PROMOTE_TO_BISHOP:
        return white_to_move ? WHITE_BISHOP : BLACK_BISHOP;
    case PROMOTE_TO_KNIGHT:
        return white_to_move ? WHITE_KNIGHT : BLACK_KNIGHT;
    default:
        throw std::invalid_argument("Invalid promotion: " + std::to_string(promotion));
    }
}

bool Move::isUndo() const
{
    return promotion == UNDO;
}

std::string Move::toString() const
{
    std::string str = getSquareName(from) + getSquareName(to);
    if (promotion & PROMOTE_TO_QUEEN)
        str += "=Q";
    else if (promotion & PROMOTE_TO_ROOK)
        str += "=R";
    else if (promotion & PROMOTE_TO_BISHOP)
        str += "=B";
    else if (promotion & PROMOTE_TO_KNIGHT)
        str += "=N";
    return str;
}

void MoveList::append(Move move)
{
    moves[size++] = move;
}

void MoveList::clear()
{
    size = 0;
}

bool MoveList::empty()
{
    return size == 0;
}

void UndoHistory::push(UndoInfo undo)
{
    undo_infos[size++] = undo;
}

UndoInfo UndoHistory::pop()
{
    return undo_infos[--size];
}

void UndoHistory::clear()
{
    size = 0;
}
