# Daraen

Daraen is a bitboard-based chess engine written in pure C++17. It can be played from the command line or connect to a GUI via the Universal Chess Interface (UCI). 

## Lichess

- **Engine rating:** _TODO_ - I am going to update this soon. I promise :)
- **Bot account:** [Daraen](https://lichess.org/@/DaraenBot)

## Engine status
### Implemented
- Bitboard board representation and move generation
    - Legal moves, castling, en passant, and promotions
    - Magic bitboards for sliding-piece attacks
    - Make/unmake move support
- Iterative-deepening alpha-beta/PV search
    - Quiescence search
    - Transposition table
    - Zobrist hashing
    - Most Valuable Victim – Least Valuable Attacker (MVV-LVA) move ordering
- Basic evaluation based on material, mobility, and pawn structure 
    - Kept intentionally simple as it will be replaced by an NNUE
- Opening-book move selection using Polyglot books
- Core UCI commands

### Planned
- Evaluation via NNUE (Efficiently Updatable Neural Network)
- Staged move exploration and ordering:
  1. Retrieve and try the best move from the previous search
  2. Generate captures and promotions
  3. Try promotions
  4. Try good captures
  5. Try killer moves
  6. Generate quiet moves
  7. Try quiet moves
  8. Try bad captures
- Stronger move ordering using Static Exchange Evaluation (SEE), killer and history heuristics

## Building and Running

Build the engine from the repository root:
```bash
g++ -std=c++17 -O3 src/*.cpp -o ./daraen
```
and run it from the repository root:
```bash
./daraen
```

Sample build tasks are provided in [`.vscode/tasks.json`](.vscode/tasks.json). 

### Modes

Use `-m <mode>` to select a mode:

| Mode | Description |
| --- | --- |
| `u` | UCI mode (default) |
| `b` | Benchmark move generation, search, and evaluation |
| `s` | Search for the best move in the current position |
| `l` | Player vs. player |
| `r` | Player vs. engine |
| `e` | Engine vs. engine |
| `p` | Perft |

Examples:

```bash
# Run perft at depth 5 from the starting position.
./daraen -m p -d 5

# Run perft and report a difference from an expected node count.
./daraen -m p -d 5 -e 4865609

# Search a FEN for 2 seconds.
./daraen -m s -f "r1bqk2r/pppp1ppp/2n2n2/8/1b1P4/2N2N2/PPP2PPP/R1BQKB1R w KQkq - 0 1" -t 2000

# Play against the engine with an opening book.
./daraen -m r -o data/opening_books/book.bin
```

More examples can be found in [`.vscode/launch.json`](.vscode/launch.json).

### Command-Line Options

| Option | Description |
| --- | --- |
| `-m <mode>` | Select a mode listed above |
| `-f <FEN>` | Set the initial position |
| `-d <depth>` | Set the search/perft depth in plies |
| `-e <nodes>` | Expected perft node count for comparison |
| `-t <milliseconds>` | Set the search time limit |
| `-o <path>` | Enable and load an opening book from this path |
| `-h` | Print command-line help |

## UCI Interface
The current UCI implementation focuses on the core commands needed to play a game. It does not yet support all UCI commands, but it is fully sufficient to connect to a GUI and play games.

Supported Commands:

| Command | Supported forms and behavior |
| --- | --- |
| `uci` | Identify the engine and print its UCI options |
| `isready` | Reply with `readyok` |
| `ucinewgame` | Stop search and reset the engine |
| `position` | `position startpos [moves <move> ...]` or `position fen <FEN> [moves <move> ...]` |
| `go` | Start a search using `depth <plies>`, `movetime <milliseconds>`, or `wtime`, `btime`, `winc`, and `binc` |
| `stop` | Stop the current search |
| `setoption` | `setoption name OwnBook value <true|false>` or `setoption name BookFile value <path>` |
| `help` | Print the built-in UCI help text |
| `quit` | Stop the engine and exit |

Search results are returned as `bestmove <move>`.

The engine advertises the following options after receiving `uci`:

| Option | Type | Default | Description |
| --- | --- | --- | --- |
| `OwnBook` | check | `true` | Enable or disable opening-book moves |
| `BookFile` | string | `../data/opening_books/book.bin` | Polyglot opening-book path |

Example Session:

```text
uci
isready
setoption name OwnBook value true
position startpos moves e2e4 e7e5
go movetime 1000
stop
quit
```


