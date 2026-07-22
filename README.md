# Chess Engine

This repository contains a chess engine written in pure C++.

## What It Does

This project implements a bitboard-based chess engine with the following pieces:

- Board representation with FEN parsing and standard starting position setup
- Legal move generation for all pieces
- Castling, en passant, and pawn promotion
- Make/unmake move support for search and game play
- Perft testing with optional divide output
- Iterative deepening negamax search with alpha-beta pruning
- Basic evaluation based on material, mobility, and pawn structure
- Zobrist hashing
- Transposition table support
- Simple move ordering heuristics
- Console play mode and a basic UCI-style interface

## Build
```bash
g++ -g src/*.cpp -o engine
```

## Run

The executable supports a few modes via command-line flags:

- `-m u` for UCI mode
- `-m c` for console play
- `-m p` for perft
- `-m s` for search

Useful options:

- `-f <FEN>` set the starting position
- `-p <ply>` set perft depth
- `-d` print divide output for perft
- `-t <ms>` set the search time limit in milliseconds
- `-h` print help

Example:

```bash
./engine -m p -p 4
```

## Repository Layout

- `src/BitBoard.*` board state, bitboard helpers, and FEN handling
- `src/MoveGenerator.*` pseudo-legal and legal move generation
- `src/Engine.*` search, perft, and game-state logic
- `src/Evaluation.*` static evaluation
- `src/ZobristHash.*` incremental hashing
- `src/TranspositionTable.*` hash table for search
- `src/UserInterface.*` CLI entry point and interaction modes

## Notes

The UCI shell is present, but some commands are still basic and not a full engine protocol implementation.
Several features (ML-based move evaluation, MCTS) are planned for the future. However, when those features will be implemented is uncertain.
