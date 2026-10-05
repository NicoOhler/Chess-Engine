#include "BitBoard.h"

void BitBoard::movePiece(Bitboard &board, Position from, Position to)
{
    board ^= (1ULL << from | 1ULL << to);
}

void BitBoard::set(Bitboard &board, Position position)
{
    board |= 1ULL << position;
}

void BitBoard::clear(Bitboard &board, Position position)
{
    board ^= 1ULL << position;
}

bool BitBoard::isSet(Bitboard board, Position position)
{
    return board & (1ULL << position);
}

std::vector<bool> BitBoard::getBits(Bitboard board)
{
    std::vector<bool> bits(NUM_SQUARES);
    for (int i = 0; i < NUM_SQUARES; i++)
        bits[i] = board & (1ULL << i);
    return bits;
}

int8 BitBoard::countSetBits(Bitboard board)
{
    return __builtin_popcountll(board);
}
Position BitBoard::clearRightmostSetBit(Bitboard &board)
{
    Position index_of_LSB = __builtin_ctzll(board);
    board &= board - 1;
    return index_of_LSB;
}

Position BitBoard::getRightmostSetBit(Bitboard &board)
{
    return __builtin_ctzll(board);
}

bool BitBoard::Board::isSquareOccupied(Position position)
{
    return occupied & (1ULL << position);
}

BitBoard::Board BitBoard::generateBoardFromFEN(std::string fen)
{
    Board board = Board(EMPTY);

    int i = 0;
    // parse board position
    for (int row = ROW_8; row >= ROW_1; row--)
    {
        for (int col = COL_A; col <= COL_H; col++)
        {
            Position square = 8 * row + col;
            char c = fen[i++];

            if (c >= '1' && c <= '8')
            {
                col += c - '1';
                continue;
            }

            Bitboard *piece = board.getBitboardByPieceSymbol(c);
            BitBoard::set(*piece, square);
            BitBoard::set(board.occupied, square);
            BitBoard::set(isupper(c) ? board.white_pieces : board.black_pieces, square);
        }
        assert(fen[i++] == (row != ROW_1 ? '/' : ' '), "Invalid FEN");
    }

    // parse side to move, castling rights, en passant square
    board.white_to_move = fen[i++] == 'w';
    if (fen[++i] == '-')
    {
        board.castling_rights = 0;
        i++;
    }
    else
        for (int j = 0; j < 4; j++)
        {
            if (fen[i] == ' ')
                break;
            switch (fen[i++])
            {
            case WHITE_KING_SYMBOL:
                board.castling_rights |= WHITE_KING_SIDE_CASTLING;
                break;
            case WHITE_QUEEN_SYMBOL:
                board.castling_rights |= WHITE_QUEEN_SIDE_CASTLING;
                break;
            case BLACK_KING_SYMBOL:
                board.castling_rights |= BLACK_KING_SIDE_CASTLING;
                break;
            case BLACK_QUEEN_SYMBOL:
                board.castling_rights |= BLACK_QUEEN_SIDE_CASTLING;
                break;
            default:
                assert(false, "Invalid FEN");
            }
        }

    assert(fen[i++] == ' ', "Invalid FEN");
    bool no_en_passant = fen[i] == '-';
    if (no_en_passant)
        board.en_passant = NO_EN_PASSANT;
    else
        board.en_passant = getSquareIndex(fen.substr(i, 2));
    i += no_en_passant ? 2 : 3;

    // parse half move clock if present
    if (fen.length() > i)
    {
        bool two_digits = fen[i + 1] != ' ';
        if (two_digits)
            board.half_move_clock = (fen[i++] - '0') * 10 + (fen[i++] - '0');
        else
            board.half_move_clock = fen[i++] - '0';
    }

    // ? is the full move number needed for anything?
    /*
    i++;
    two_digits = fen.length() != i + 1;
    if (two_digits)
    board.full_move_number = (fen[i++] - '0') * 10 + (fen[i++] - '0');
    else
    board.full_move_number = fen[i] - '0';
    */
    return board;
}

Bitboard *BitBoard::Board::getBitboardByPiece(Piece piece)
{
    switch (piece)
    {
    case WHITE_PAWN:
        return &white_pawns;
    case WHITE_ROOK:
        return &white_rooks;
    case WHITE_KNIGHT:
        return &white_knights;
    case WHITE_BISHOP:
        return &white_bishops;
    case WHITE_QUEEN:
        return &white_queens;
    case WHITE_KING:
        return &white_king;
    case BLACK_PAWN:
        return &black_pawns;
    case BLACK_ROOK:
        return &black_rooks;
    case BLACK_KNIGHT:
        return &black_knights;
    case BLACK_BISHOP:
        return &black_bishops;
    case BLACK_QUEEN:
        return &black_queens;
    case BLACK_KING:
        return &black_king;
    default:
        assert(false, "Invalid piece");
    }
    return nullptr;
}

Bitboard *BitBoard::Board::getBitboardByPieceSymbol(Piece piece)
{
    switch (piece)
    {
    case WHITE_PAWN_SYMBOL:
        return &white_pawns;
    case WHITE_ROOK_SYMBOL:
        return &white_rooks;
    case WHITE_KNIGHT_SYMBOL:
        return &white_knights;
    case WHITE_BISHOP_SYMBOL:
        return &white_bishops;
    case WHITE_QUEEN_SYMBOL:
        return &white_queens;
    case WHITE_KING_SYMBOL:
        return &white_king;
    case BLACK_PAWN_SYMBOL:
        return &black_pawns;
    case BLACK_ROOK_SYMBOL:
        return &black_rooks;
    case BLACK_KNIGHT_SYMBOL:
        return &black_knights;
    case BLACK_BISHOP_SYMBOL:
        return &black_bishops;
    case BLACK_QUEEN_SYMBOL:
        return &black_queens;
    case BLACK_KING_SYMBOL:
        return &black_king;
    default:
        assert(false, "Invalid piece");
    }
    return nullptr;
}

Piece BitBoard::Board::getPieceAt(Position position)
{
    Bitboard mask = 1ULL << position;
    if (white_pawns & mask)
        return WHITE_PAWN;
    if (white_rooks & mask)
        return WHITE_ROOK;
    if (white_knights & mask)
        return WHITE_KNIGHT;
    if (white_bishops & mask)
        return WHITE_BISHOP;
    if (white_queens & mask)
        return WHITE_QUEEN;
    if (white_king & mask)
        return WHITE_KING;
    if (black_pawns & mask)
        return BLACK_PAWN;
    if (black_rooks & mask)
        return BLACK_ROOK;
    if (black_knights & mask)
        return BLACK_KNIGHT;
    if (black_bishops & mask)
        return BLACK_BISHOP;
    if (black_queens & mask)
        return BLACK_QUEEN;
    if (black_king & mask)
        return BLACK_KING;
    return EMPTY;
}

void BitBoard::printBitboard(Bitboard board)
{
    std::vector<bool> bits = getBits(board);
    std::cout << std::endl;
    for (Position row = NUM_ROWS - 1; row >= 0; row--)
    {
        std::cout << row + 1 << " ";
        for (Position col = 0; col < NUM_COLS; col++)
            std::cout << bits[row * NUM_COLS + col] ? "1" : "0";
        std::cout << std::endl;
    }
    std::cout << "  ";
    for (Position col = 0; col < NUM_COLS; col++)
        std::cout << (char)('a' + col);
    std::cout << std::endl
              << std::endl;
}

void BitBoard::printGameState(Board board)
{
    printBoard(board);
    if (board.en_passant != NO_EN_PASSANT)
        std::cout << "En passant square: " << getSquareName(board.en_passant) << std::endl;
    std::cout << "Half move counter: " << std::to_string(board.half_move_clock) << std::endl;
    printCastlingRights(board);
    std::cout << (board.white_to_move ? "White" : "Black") << "'s turn" << std::endl
              << std::endl;
}

void BitBoard::printBoard(Board board)
{
    char board_to_print[8][8];
    placePiecesOnBoard(board, board_to_print);
    std::cout << std::endl
              << "  a b c d e f g h" << std::endl;
    for (char i = 0; i < 8; i++)
    {
        std::cout << 8 - i << " ";
        for (char j = 0; j < 8; j++)
            std::cout << board_to_print[i][j] << " ";
        std::cout << 8 - i << std::endl;
    }
    std::cout << "  a b c d e f g h" << std::endl
              << std::endl;
}

void BitBoard::printCastlingRights(Board board)
{
    std::cout << "Castling rights: ";
    if (board.castling_rights & WHITE_KING_SIDE_CASTLING)
        std::cout << "K";
    if (board.castling_rights & WHITE_QUEEN_SIDE_CASTLING)
        std::cout << "Q";
    if (board.castling_rights & BLACK_KING_SIDE_CASTLING)
        std::cout << "k";
    if (board.castling_rights & BLACK_QUEEN_SIDE_CASTLING)
        std::cout << "q";
    std::cout << std::endl;
}

void BitBoard::placePiecesOnBoard(Board board, char board_to_print[8][8])
{
    Bitboard pieces[12] = {board.white_pawns, board.white_rooks, board.white_knights, board.white_bishops, board.white_queens, board.white_king,
                           board.black_pawns, board.black_rooks, board.black_knights, board.black_bishops, board.black_queens, board.black_king};
    Piece piece_types[12] = {WHITE_PAWN_SYMBOL, WHITE_ROOK_SYMBOL, WHITE_KNIGHT_SYMBOL, WHITE_BISHOP_SYMBOL, WHITE_QUEEN_SYMBOL, WHITE_KING_SYMBOL,
                             BLACK_PAWN_SYMBOL, BLACK_ROOK_SYMBOL, BLACK_KNIGHT_SYMBOL, BLACK_BISHOP_SYMBOL, BLACK_QUEEN_SYMBOL, BLACK_KING_SYMBOL};
    for (char i = 0; i < 8; i++)
        for (char j = 0; j < 8; j++)
            board_to_print[i][j] = ' ';

    for (int i = 0; i < 12; i++)
    {
        while (pieces[i])
        {
            Position position = BitBoard::clearRightmostSetBit(pieces[i]);
            board_to_print[7 - position / 8][modPow2(position, 8)] = piece_types[i];
        }
    }
}
