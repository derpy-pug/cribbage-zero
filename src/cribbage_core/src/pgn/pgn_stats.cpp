#include "pgn/pgn_stats.h"

namespace cribbage {

PGN_Stats::PGN_Stats(const PGN& pgn) {
    pgns = {pgn};
}

PGN_Stats::PGN_Stats(const std::vector<PGN>& pgns) {
    this->pgns = pgns;
}


}  // namespace cribbage
