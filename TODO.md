### TODO
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
+ setup Sequential Probability Ratio Test
    + evaluates whether a patch makes the engine stronger compared to live version
    + e.g. using fast-chess
