### Engine Strength Improvements
+ evaluation using NNUE
    + read up on them
    + check for state of the art alternatives
    + define its input output formats
    + fetch/create dataset from played games
    + setup training environment 
+ reduce sizeof(Move)
    + use single 16bit uint for from + to + promotion
+ staged move generation/ordering/exploration
    + retrieve best previous move and check it
    + generate captures and promotions
    + promotions
    + good captures
    + killer moves
    + generate quiet moves
    + quiet moves
    + bad captures
+ maybe improve move ordering via
    + Static Exchange Evaluation (SEE)
        + read wiki
    + killer heuristic
        + orders quiet (non-capture) moves that cause beta cutoffs
        + reuse information from neighbor branches
        + read wiki
    + history heuristic
        + track how often quiet moves cause cutoffs
        + prioritize those
        + requires decay mechanism
+ improve quiescence search
    + consider checks + en passant
    + maybe make use of transposition table?
    + may benefit from pruning
        + e.g. SEE < 0

### Nice-To-Haves for Later
+ update README.md
+ setup Sequential Probability Ratio Test
    + evaluates whether a patch makes the engine stronger compared to live version
    + e.g. using fast-chess
+ estimate ELO
    + connect to lichess-bot api
    + https://anmols.bearblog.dev/how-to-determine-chess-bot-elo-lichess/
+ come up with a good name
+ endgame tablebase 
    + consists of precomputed endgame positions with known outcomes
    + purpose
        + allows engine to identify known positions and guarantee best moves
        + avoid unnecessary searching
        + force wins using fewer moves
        + endgames are difficult even for engines
    + quite large => partitioned into separate files based on
        + side to move
        + remaining materials
    + entries provide
        + expected outcome (win/draw/loss)
        + number of remaining moves
        + best move
    + castling rights are typically disregarded
