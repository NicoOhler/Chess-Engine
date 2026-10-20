#include "Board.h"

Board Board::generateBoardFromFEN(const std::string &fen)
{
    Board board = Board(EMPTY);
    int i = 0;
    // parse board position
    for (int row = ROW_8; row >= ROW_1; row--)
    {
        for (int col = COL_A; col <= COL_H; col++)
        {
            Position square = NUM_COLS * row + col;
            char c = fen[i++];

            if (c >= '1' && c <= '8')
            {
                col += c - '1';
                continue;
            }

            Piece piece = getPieceFromSymbol(c);
            board.pieces[square] = piece;
            set(board.piece_bitboards[piece], square);
            set(board.occupied, square);
            set(board.colors[isupper(c) ? WHITE : BLACK], square);
        }
        if (fen[i++] != (row != ROW_1 ? '/' : ' '))
            throw std::invalid_argument("Invalid FEN: Expected '/' or ' ' after row " + std::to_string(row));
    }

    // parse side to move, castling rights, en passant square
    board.white_to_move = fen[i++] == 'w';
    if (fen[++i] == '-')
    {
        board.castling_rights = 0;
        i++;
    }
    else
        for (int j = 0; j < NUM_CASTLING_RIGHTS; j++)
        {
            if (fen[i] == ' ')
                break;
            switch (fen[i++])
            {
            case WHITE_KING_SYMBOL:
                board.castling_rights |= WHITE_KING_CASTLING;
                break;
            case WHITE_QUEEN_SYMBOL:
                board.castling_rights |= WHITE_QUEEN_CASTLING;
                break;
            case BLACK_KING_SYMBOL:
                board.castling_rights |= BLACK_KING_CASTLING;
                break;
            case BLACK_QUEEN_SYMBOL:
                board.castling_rights |= BLACK_QUEEN_CASTLING;
                break;
            default:
                throw std::invalid_argument("Invalid FEN: Invalid castling rights character: " + std::string(1, fen[i - 1]));
            }
        }

    if (fen[i++] != ' ')
        throw std::invalid_argument("Invalid FEN: Expected space after castling rights");

    bool no_en_passant = fen[i] == '-';
    if (no_en_passant)
        board.en_passant = NO_EN_PASSANT;
    else
        board.en_passant = getSquareIndex(fen.substr(i, 2));
    i += no_en_passant ? 2 : 3;

    // parse half move clock if present
    board.half_move_clock = 0;
    while (i < fen.length() && isdigit(fen[i]))
        board.half_move_clock = board.half_move_clock * 10 + (fen[i++] - '0');

    // ? is the full move number needed for anything?
    return board;
}

bool Board::isSquareOccupied(Position position) const
{
    return occupied & (1ULL << position);
}

void Board::printGameState(const Board &board)
{
    Board::printBoard(board);
    if (board.en_passant != NO_EN_PASSANT)
        std::cout << "En passant square: " << getSquareName(board.en_passant) << std::endl;
    std::cout << "Half move counter: " << std::to_string(board.half_move_clock) << std::endl;
    Board::printCastlingRights(board);
    std::cout << (board.white_to_move ? "White" : "Black") << "'s turn" << std::endl
              << std::endl;
}

void Board::printBoard(const Board &board)
{
    std::cout << std::endl
              << "  a b c d e f g h" << std::endl;
    for (char row = 0; row < NUM_ROWS; row++)
    {
        std::cout << NUM_ROWS - row << " ";
        for (char col = 0; col < NUM_COLS; col++)
        {
            Position square = NUM_COLS * (NUM_ROWS - row - 1) + col;
            std::cout << PIECE_SYMBOLS[board.pieces[square]] << " ";
        }
        std::cout << NUM_ROWS - row << std::endl;
    }
    std::cout << "  a b c d e f g h" << std::endl
              << std::endl;
}

void Board::printCastlingRights(const Board &board)
{
    std::cout << "Castling rights: ";
    if (board.castling_rights & WHITE_KING_CASTLING)
        std::cout << "K";
    if (board.castling_rights & WHITE_QUEEN_CASTLING)
        std::cout << "Q";
    if (board.castling_rights & BLACK_KING_CASTLING)
        std::cout << "k";
    if (board.castling_rights & BLACK_QUEEN_CASTLING)
        std::cout << "q";
    std::cout << std::endl;
}
