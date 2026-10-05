### Important Features
+ split up move generation
+ improve move ordering
    + move ordering is typically staged
        + retrieve best previous move and check it
        + generate captures and promotions
        + promotions
        + good captures
        + killer moves
        + generate quiet moves
        + quiet moves
        + bad captures
    + optionally Static Exchange Evaluation (SEE)
        + read wiki
    + optionally killer heuristic
        + orders quiet (non-capture) moves that cause beta cutoffs
        + reuse information from neighbor branches
        + read wiki
    + optionally history heuristic
        + track how often quiet moves cause cutoffs
        + prioritize those
        + requires decay mechanism
+ quiescence search
    + requires at least basic move ordering to avoid search explosion
    + score of current position is used as lower bound
        + assumes that own next move does not worsen the position (i.e. no zugzwang)
    + additions
        + consider checks + en passant
        + maybe make use of transposition table?
        + may benefit from pruning
            + e.g. SEE < 0
+ evaluation using NNUE
    + read up on them
    + check for state of the art alternatives
    + define its input output formats
    + fetch/create dataset from played games
    + setup training environment 

### Nice-To-Haves for Later
+ time management
    + cap the search time at remaining time/20 + increment/2
    + no more time left => ensure consistent state
        + throw away result of current iteration?
        + throw away pending updates to transposition table on early exit?
+ UCI support
    + focus on subset required for playing, connecting to GUI and testing
+ setup Sequential Probability Ratio Test
    + evaluates whether a patch makes the engine stronger compared to live version
    + e.g. using fast-chess
+ estimate ELO
    + requires UCI support
    + connect to lichess-bot api
    + https://anmols.bearblog.dev/how-to-determine-chess-bot-elo-lichess/
+ connect engine to GUI via UCI
+ come up with a good name
+ opening book
    + play probabilistic book moves at first
        + saves compute time
        + prevents "local minima"
        + even large search depths cannot determine "best moves" 
    + use well established book format
    + stores common position in list sorted by hash
        + hash (board representation)
        + occurences
        + games won by black/white
        + games drawn
    + integrate from existing textual database for simplicity
        + extract FEN
        + create board representation
        + compute its hash
        + populate own database
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
