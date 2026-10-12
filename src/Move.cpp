#include "Move.h"

bool Move::operator==(const Move &rhs)
{
    return from == rhs.from && to == rhs.to && piece == rhs.piece && promotion_and_castling == rhs.promotion_and_castling;
}

bool Move::operator!=(const Move &rhs)
{
    return from != rhs.from || to != rhs.to || piece != rhs.piece || promotion_and_castling != rhs.promotion_and_castling;
}

bool Move::isPromotion() const
{
    return promotion_and_castling & (PROMOTE_TO_QUEEN | PROMOTE_TO_ROOK | PROMOTE_TO_BISHOP | PROMOTE_TO_KNIGHT);
}

bool Move::isCastling() const
{
    return promotion_and_castling & (WHITE_KING_CASTLING | WHITE_QUEEN_CASTLING | BLACK_KING_CASTLING | BLACK_QUEEN_CASTLING);
}

std::string Move::toString() const
{
    std::string str = getSquareName(from) + getSquareName(to);
    if (promotion_and_castling & PROMOTE_TO_QUEEN)
        str += "=Q";
    else if (promotion_and_castling & PROMOTE_TO_ROOK)
        str += "=R";
    else if (promotion_and_castling & PROMOTE_TO_BISHOP)
        str += "=B";
    else if (promotion_and_castling & PROMOTE_TO_KNIGHT)
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
    undo_info[size++] = undo;
}

UndoInfo UndoHistory::pop()
{
    return undo_info[--size];
}

void UndoHistory::clear()
{
    size = 0;
}
