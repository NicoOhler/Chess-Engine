#include "ZobristHash.h"

ZobristHash::ZobristHash()
{
    // initialize xor keys with official polyglot keys for reproducibility and compatibility with openings books and other engines
    int offset = 0;
    for (int piece = 0; piece < NUM_PIECES; piece++)
        for (int square = 0; square < NUM_SQUARES; square++)
            piece_at_square[square][piece] = polyglot_keys[offset++];
    for (int i = 0; i < NUM_CASTLING_RIGHTS; i++)
        castling_rights[i] = polyglot_keys[offset++];
    for (int i = 0; i < NUM_COLS; i++)
        en_passant_file[i] = polyglot_keys[offset++];
    is_whites_turn = polyglot_keys[offset++];
}

Hash ZobristHash::computeInitialHash(Board &board)
{
    Hash hash = 0;
    // add all pieces
    for (int square = 0; square < NUM_SQUARES; square++)
    {
        Piece piece = board.pieces[square];
        if (piece != EMPTY)
            hash ^= piece_at_square[square][piece];
    }

    // indicate whose turn it is
    if (board.white_to_move)
        hash ^= is_whites_turn;

    // add en passant
    if (board.en_passant != NO_EN_PASSANT)
        hash ^= en_passant_file[modPow2(board.en_passant, NUM_COLS)];

    // add castling rights
    if (board.castling_rights & WHITE_KING_CASTLING)
        hash ^= castling_rights[0];
    if (board.castling_rights & WHITE_QUEEN_CASTLING)
        hash ^= castling_rights[1];
    if (board.castling_rights & BLACK_KING_CASTLING)
        hash ^= castling_rights[2];
    if (board.castling_rights & BLACK_QUEEN_CASTLING)
        hash ^= castling_rights[3];
    return hash;
}

Hash ZobristHash::updateHash(Board &board, Move move, UndoInfo undo)
{
    Hash &hash = board.hash;
    Piece moving_piece = board.pieces[move.to]; // assume move has already been made => piece is at destination square
    bool white_to_move = !board.white_to_move;

    // toggle whose turn it is
    hash ^= is_whites_turn;

    // remove piece from previous position
    hash ^= piece_at_square[move.from][moving_piece];

    // remove previous en passant
    bool is_en_passant = (move.to == undo.en_passant) && (moving_piece == (white_to_move ? WHITE_PAWN : BLACK_PAWN));
    if (undo.en_passant != NO_EN_PASSANT)
    {
        hash ^= en_passant_file[modPow2(undo.en_passant, NUM_COLS)];
        // apply en passant capture
        if (is_en_passant)
            hash ^= piece_at_square[move.to + (white_to_move ? DOWN : UP)][white_to_move ? BLACK_PAWN : WHITE_PAWN];
    }

    // add new en passant square
    if (board.en_passant != NO_EN_PASSANT)
        hash ^= en_passant_file[modPow2(move.to, NUM_COLS)];

    // remove captured piece
    if (undo.captured_piece != EMPTY && !is_en_passant)
        hash ^= piece_at_square[move.to][undo.captured_piece];

    // add piece to new position
    Piece piece = move.isPromotion() ? move.getPromotionPiece(white_to_move) : moving_piece;
    hash ^= piece_at_square[move.to][piece];

    // additionally move rook if castling
    bool is_king_move = moving_piece == (white_to_move ? WHITE_KING : BLACK_KING);
    if (move.isCastling(is_king_move))
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
        if ((undo.castling_rights ^ board.castling_rights) & WHITE_KING_CASTLING)
            hash ^= castling_rights[0];
        if ((undo.castling_rights ^ board.castling_rights) & WHITE_QUEEN_CASTLING)
            hash ^= castling_rights[1];
        if ((undo.castling_rights ^ board.castling_rights) & BLACK_KING_CASTLING)
            hash ^= castling_rights[2];
        if ((undo.castling_rights ^ board.castling_rights) & BLACK_QUEEN_CASTLING)
            hash ^= castling_rights[3];
    }

    return hash;
}
