#include "statistics/stats/ScoreDistributionTable.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace cribbage {

ScoreDistributionTable::ScoreDistributionTable()
    : possible_min(0), possible_max(29), min_score(29), max_score(0) {
    dist_table.resize(30);
}

ScoreDistributionTable::ScoreDistributionTable(int min, int max)
    : possible_min(min), possible_max(max), min_score(max), max_score(min) {
    dist_table.resize(max - min + 1);
}

float& ScoreDistributionTable::get_table_value(int score) {
    if (score < possible_min || score > possible_max) {
        std::cerr << "Score out of range: " << score << std::endl;
        exit(1);
    }
    int score_index = score - possible_min;
    return dist_table[score_index];
}

const float& ScoreDistributionTable::get_table_value(int score) const {
    if (score < possible_min || score > possible_max) {
        std::cerr << "Score out of range: " << score << std::endl;
        exit(1);
    }
    int score_index = score - possible_min;
    return dist_table[score_index];
}

float ScoreDistributionTable::calc_mean() const {
    float mean = 0;
    for (int i = min_score; i <= max_score; ++i) {
        mean += i * get_table_value(i);
    }
    return mean;
}

int ScoreDistributionTable::calc_median() const {
    float cummulative = 0;
    for (int i = min_score; i <= max_score; ++i) {
        cummulative += get_table_value(i);
        if (cummulative >= 0.5) {
            return i;
        }
    }
    return 0;
}

int ScoreDistributionTable::calc_max() const {
    for (int i = max_score; i >= min_score; --i) {
        if (get_table_value(i) > 0) {
            return i;
        }
    }
    return 0;
}

int ScoreDistributionTable::calc_min() const {
    for (int i = min_score; i <= max_score; ++i) {
        if (get_table_value(i) > 0) {
            return i;
        }
    }
    return 0;
}

float ScoreDistributionTable::calc_variance() const {
    float mean = calc_mean();
    float variance = 0;
    for (int i = min_score; i <= max_score; ++i) {
        variance += (i - mean) * (i - mean) * get_table_value(i);
    }
    return variance;
}

float ScoreDistributionTable::calc_std_dev() const {
    return std::sqrt(calc_variance());
}

float ScoreDistributionTable::prob_score(int score) const {
    if (score < min_score || score > max_score) {
        return 0;
    }
    return get_table_value(score);
}

float ScoreDistributionTable::prob_score_cummulative(int score) const {
    if (score < min_score) {
        return 0;
    }
    if (score >= max_score) {
        return 1;
    }
    float cummulative = 0;
    for (int i = min_score; i <= score; ++i) {
        cummulative += get_table_value(i);
    }
    return cummulative;
}

float ScoreDistributionTable::prob_score_between(int score1, int score2) const {
    if (score1 > score2) {
        std::swap(score1, score2);
    }
    if (score1 < min_score) {
        score1 = min_score;
    }
    if (score2 > max_score) {
        score2 = max_score;
    }
    float cummulative = 0;
    for (int i = score1; i <= score2; ++i) {
        cummulative += get_table_value(i);
    }
    return cummulative;
}

float& ScoreDistributionTable::operator[](int score) {
    if (score < possible_min || score > possible_max) {
        std::cerr << "Score out of range: " << score << std::endl;
        exit(1);
    }
    return get_table_value(score);
}

const float& ScoreDistributionTable::operator[](int score) const {
    if (score < possible_min || score > possible_max) {
        std::cerr << "Score out of range: " << score << std::endl;
        exit(1);
    }
    return get_table_value(score);
}

void ScoreDistributionTable::clear() {
    for (int i = 0; i < (int)dist_table.size(); ++i) {
        dist_table[i] = 0;
    }
}

void ScoreDistributionTable::normalize() {
    float total = 0;
    for (int i = possible_min; i <= possible_max; ++i) {
        auto prob = dist_table[i - possible_min];
        if (prob > 0) {
            total += prob;
            if (i < min_score) {
                min_score = i;
            }
            if (i > max_score) {
                max_score = i;
            }
        }
    }
    for (int i = min_score; i <= max_score; ++i) {
        dist_table[i - min_score] /= total;
    }
}


} // namespace cribbage
