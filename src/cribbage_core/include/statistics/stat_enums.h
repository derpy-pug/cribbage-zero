#pragma once

namespace cribbage {

enum class ScoreType { HAND, CRIB, COMBINED };

enum class Statistic {
    MEAN,
    MEDIAN,
    VARIANCE,
    STD_DEV,
    MAX,
    MIN,
};

}  // namespace cribbage
