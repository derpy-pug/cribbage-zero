#pragma once

#include <string>

inline std::string help() {
    return R"(
Command line arguments:

cribbage help
  Print this help message.

cribbage discards [OPTIONS]
    -h, --hand HAND
      HAND is "AH 2H 3H 4H 5H 6H" for example.
      6 cards, not necessarily separated by spaces.
      Each card is a rank followed by a suit (S, C, D, H)
      Rank: A, 1, 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K
      Examples: 1H, AH, 2H, TH, TH, JH, QH, KH
      A and 1 is the same thing.
      Leave it blank for a random hand.

    --dealer DEALER
      DEALER is a truthy value (1, true, True, T, t, yes, Yes, Y, y)
      or a falsey value (0, false, False, F, f, no, No, N, n)
      If you don't specify a dealer, it will be random.

    -c, --cut CUT
      If you know the cut card, you can specify it here.
      CUT is a rank followed by a suit (S, C, D, H)

    -d, --discards DISCARDS
      DISCARDS is two cards like "AH 2H"
      Can ONLY be used if you specify the hand

    -s, --sort-by SORT_BY
      SORT_BY is the following:
       - "<score_type> <statistic>"
       - <statistic> is one of "max", "min", "mean", "median", "variance", "std_dev"
       - <score_type> is one of "hand", "crib", "combined"

    -t, --table TABLE
      TABLE is the name of the table to load.
      If you don't specify a table, it will look in the default directory.
      If it doesn't find a table, it will generate one.

    --top-discards TOP_DISCARDS
      TOP_DISCARDS is the number of top discards to print.
      Must be between 1 and 15.

cribbage game
    -n, --num_games NUM_GAMES
    NUM_GAMES is the number of games to play during this command.

    --output-type TYPE
    TYPE is how the the games should be output to stdout.
    TYPE can be:
        - "pgn" : prints the pgns of all games
        - "summary" : prints a summary of all games


Examples:
  most used:
  ./cribbage discards -h "AH 2H 3H 4H 5H 6H" --dealer 1
  ./cribbage discards -h "AH 2H 3H 4H 5H 6H" --dealer N -c AH --top-discards 3
  ./cribbage discards -h "AH 2H 3H 4H 5H 6H" --dealer 0 -d "AH 2H"
  ./cribbage game

  other:
  ./cribbage discards -h "AH 2H 3H 4H 5H 6H" -c "7H" -d "AH 2H" -s "median combined"
  ./cribbage discards -h "AH 2H 3H 4H 5H 6H" -c "7H" -d "AH 2H" -s "mean hand" -t "stat_vs_stat"
)";
}
