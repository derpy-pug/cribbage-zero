#pragma once

#include <vector>

#include "pgn/game_pgn.h"

namespace cribbage {

class PGN_Stats {
  public:
    PGN_Stats() = default;
    PGN_Stats(const PGN& pgn);
    PGN_Stats(const std::vector<PGN>& pgns);

    void print_stats() const;

  private:
    std::vector<PGN> pgns;
};

}  // namespace cribbage
