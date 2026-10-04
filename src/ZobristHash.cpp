#include "ZobristHash.h"

void ZobristHash::initialize()
{
    std::mt19937 mt{RAND_SEED};
    for (int square = 0; square < 64; square++)
        for (int piece = 0; piece < 12; piece++)
            piece_at_square[square][piece] = mt();
    whites_turn = mt();
    for (int i = 0; i < 4; i++)
        castling_rights[i] = mt();
    for (int i = 0; i < 8; i++)
        en_passant_file[i] = mt();
}

uint64 ZobristHash::computeInitialHash(Board &board)
{
    uint64 hash = 0;
    // add all pieces
    for (int square = 0; square < 64; square++)
    {
        Piece piece = board.getPieceAt(square);
        if (piece != EMPTY)
            hash ^= piece_at_square[square][piece];
    }

    // indicate whose turn it is
    if (board.white_to_move)
        hash ^= whites_turn;

    // add en passant
    if (board.en_passant != NO_EN_PASSANT)
        hash ^= en_passant_file[board.en_passant % 8];

    // add castling rights
    if (board.castling_rights & WHITE_KING_SIDE_CASTLING)
        hash ^= castling_rights[0];
    if (board.castling_rights & WHITE_QUEEN_SIDE_CASTLING)
        hash ^= castling_rights[1];
    if (board.castling_rights & BLACK_KING_SIDE_CASTLING)
        hash ^= castling_rights[2];
    if (board.castling_rights & BLACK_QUEEN_SIDE_CASTLING)
        hash ^= castling_rights[3];
    return hash;
}

uint64 ZobristHash::updateHash(Board &board, Move move, UndoInfo undo)
{
    uint64 &hash = board.hash;
    bool white_to_move = move.piece <= WHITE_KING;

    // toggle whose turn it is
    hash ^= whites_turn;

    // remove piece from previous position
    hash ^= piece_at_square[move.from][move.piece];

    // remove previous en passant
    bool is_pawn_move = move.piece == (white_to_move ? WHITE_PAWN : BLACK_PAWN);
    bool is_en_passant = is_pawn_move && (move.to == undo.en_passant);
    if (undo.en_passant != NO_EN_PASSANT)
    {
        hash ^= en_passant_file[undo.en_passant % 8];
        // apply en passant capture
        if (is_en_passant)
            hash ^= piece_at_square[move.to + (white_to_move ? DOWN : UP)][white_to_move ? BLACK_PAWN : WHITE_PAWN];
    }

    // add new en passant square
    if (is_pawn_move && (abs(move.from - move.to) == 16))
        hash ^= en_passant_file[move.to % 8];

    // remove captured piece
    if (undo.captured_piece != EMPTY && !is_en_passant)
        hash ^= piece_at_square[move.to][undo.captured_piece];

    // add piece to new position
    if (move.promotion)
        hash ^= piece_at_square[move.to][move.promotion];
    else
        hash ^= piece_at_square[move.to][move.piece];

    // handle castling moves by moving the rook
    if (move.castling)
    {
        // remove old rook position and add new rook position
        bool king_side_castling = move.to > move.from;
        Piece rook = white_to_move ? WHITE_ROOK : BLACK_ROOK;
        Position old_rook_position = move.from + (king_side_castling ? 3 * RIGHT : 4 * LEFT);
        hash ^= piece_at_square[old_rook_position][rook];
        Position new_rook_position = move.from + (king_side_castling ? RIGHT : LEFT);
        hash ^= piece_at_square[new_rook_position][rook];
    }

    // remove castling rights on change
    if (undo.castling_rights != board.castling_rights)
    {
        if ((undo.castling_rights ^ board.castling_rights) & WHITE_KING_SIDE_CASTLING)
            hash ^= castling_rights[0];
        if ((undo.castling_rights ^ board.castling_rights) & WHITE_QUEEN_SIDE_CASTLING)
            hash ^= castling_rights[1];
        if ((undo.castling_rights ^ board.castling_rights) & BLACK_KING_SIDE_CASTLING)
            hash ^= castling_rights[2];
        if ((undo.castling_rights ^ board.castling_rights) & BLACK_QUEEN_SIDE_CASTLING)
            hash ^= castling_rights[3];
    }

    return hash;
}
