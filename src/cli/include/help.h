#include <string>

inline std::string help() {
  return R"(
Command line arguments:
-h, --hand=HAND
  HAND is "AH 2H 3H 4H 5H 6H <Y/N>" for example.
  6 cards, not necessarily separated by spaces, and a Y/N.
  Each card is a rank followed by a suit (S, C, D, H)
  Rank: A, 1, 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K
  Examples: 1H, AH, 2H, TH, TH, JH, QH, KH
  A and 1 is the same thing.
  The last character signifies whether you are the dealer or not.
  Leave it blank for a random hand and dealer.

-c, --cut=CUT
  If you know the cut card, you can specify it here.
  CUT is a rank followed by a suit (S, C, D, H)

-d, --discards=DISCARDS
  DISCARDS is two cards like "AH 2H"
  Can ONLY be used if you specify the hand

-s, --sort-by=SORT_BY
  SORT_BY is the following:
   - "<statistic> <score_type>"
   - <statistic> is one of "max", "min", "mean", "median", "variance", "std_dev"
   - <score_type> is one of "hand", "crib", "combined"

-t, --table=TABLE
  TABLE is the name of the table to load.
  If you don't specify a table, it will look in the default directory.
  If it doesn't find a table, it will generate one.

--top-discards=TOP_DISCARDS
  TOP_DISCARDS is the number of top discards to print.
  Must be between 1 and 15.

--help
  Print this help message.


Examples:
  most used:
  ./cribbage -h "AH 2H 3H 4H 5H 6H Y"
  ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -c --top-discards=3
  ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -d "AH 2H"

  other:
  ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -c "7H" -d "AH 2H" -s "median combined"
  ./cribbage -h "AH 2H 3H 4H 5H 6H Y" -c "7H" -d "AH 2H" -s "mean hand" -t "stat_vs_stat"
)";
}

