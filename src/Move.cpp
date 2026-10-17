#include "Move.h"

Move::Move(Move16 encoded_move)
{
    Position to_col = encoded_move & 0x7;          // bits 0-2
    Position to_row = (encoded_move >> 3) & 0x7;   // bits 3-5
    Position from_col = (encoded_move >> 6) & 0x7; // bits 6-8
    Position from_row = (encoded_move >> 9) & 0x7; // bits 9-11
    to = to_row * NUM_COLS + to_col;
    from = from_row * NUM_COLS + from_col;
    promotion = (encoded_move >> 12) & 0x7; // bits 12-14
}

Move::Move(std::string move_string)
{
    if (move_string.length() < 4)
        throw std::invalid_argument("Invalid move string: " + move_string);
    from = getSquareIndex(move_string.substr(0, 2));
    to = getSquareIndex(move_string.substr(2, 2));
    promotion = NO_PROMOTION;
    if (move_string.length() == 5)
    {
        if (move_string[4] == BLACK_QUEEN_SYMBOL || move_string[4] == WHITE_QUEEN_SYMBOL)
            promotion = PROMOTE_TO_QUEEN;
        else if (move_string[4] == BLACK_ROOK_SYMBOL || move_string[4] == WHITE_ROOK_SYMBOL)
            promotion = PROMOTE_TO_ROOK;
        else if (move_string[4] == BLACK_BISHOP_SYMBOL || move_string[4] == WHITE_BISHOP_SYMBOL)
            promotion = PROMOTE_TO_BISHOP;
        else if (move_string[4] == BLACK_KNIGHT_SYMBOL || move_string[4] == WHITE_KNIGHT_SYMBOL)
            promotion = PROMOTE_TO_KNIGHT;
        else
            throw std::invalid_argument("Invalid promotion character: " + std::string(1, move_string[4]));
    }
}

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
