#include "statistics.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

#define TABLE_DIR "tables/"

namespace cribbage {

constexpr static int default_num_sim_discards = 2 << 19; 

const static float default_stat_vs_stat_my_crib[13][13] = {
  {0.013979, 0.0284529, 0.0315924, 0.0317945, 0.00486279, 0.00565147,
   0.00692844, 0.00679111, 0.00712204, 0.00541401, 0.00761318, 0.00885773,
   0.0117083},
  {0, 0.0169525, 0.0442209, 0.0211363, 0.00473213, 0.00669384, 0.00923443,
   0.00796509, 0.00788975, 0.00536251, 0.00742912, 0.00865555, 0.012248},
  {0, 0, 0.0145931, 0.0221853, 0.00681114, 0.00627518, 0.00733852, 0.00943089,
   0.00707245, 0.00491714, 0.0067091, 0.00858021, 0.0122013},
  {0, 0, 0, 0.0106754, 0.00629139, 0.00302315, 0.00654888, 0.00491333,
   0.00529861, 0.00484371, 0.0057478, 0.0074749, 0.0108757},
  {0, 0, 0, 0, 0.00870228, 0.0059433, 0.00690365, 0.00428581, 0.00424576,
   0.0120583, 0.0203514, 0.0223598, 0.0224638},
  {0, 0, 0, 0, 0, 0.00878906, 0.0160666, 0.0151358, 0.0169592, 0.00271988,
   0.0038271, 0.00378036, 0.0053997},
  {0, 0, 0, 0, 0, 0, 0.0100336, 0.0265827, 0.0111609, 0.00276184, 0.0042038,
   0.00437927, 0.00682926},
  {0, 0, 0, 0, 0, 0, 0, 0.00892735, 0.0179653, 0.00785637, 0.00357723,
   0.00368023, 0.00394154},
  {0, 0, 0, 0, 0, 0, 0, 0, 0.00800228, 0.0113173, 0.00733089, 0.00389767,
   0.00456524},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00864506, 0.0234957, 0.0120077, 0.00799084},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0116854, 0.0313263, 0.023016},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0136251, 0.0307255},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.0153828}};
const static float default_stat_vs_stat_opp_crib[13][13] = {
  {0.0029335, 0.0184946, 0.0121651, 0.00640774, 0.00148582, 0.0124569,
   0.0138016, 0.0201616, 0.0201998, 0.0178318, 0.0135164, 0.0243034, 0.0270805},
  {0, 0.00273418, 0.00302505, 0.0103493, 0.0018301, 0.0107327, 0.0158634,
   0.018096, 0.0178413, 0.0127363, 0.00943565, 0.0167513, 0.0189104},
  {0, 0, 0.00187778, 0.00573826, 0.000852585, 0.0128613, 0.010726, 0.0139341,
   0.0134163, 0.0115509, 0.00909233, 0.0151405, 0.0171585},
  {0, 0, 0, 0.00158119, 0.000750542, 0.0037365, 0.0120773, 0.0107899, 0.0128088,
   0.0118656, 0.00966454, 0.0162992, 0.0183659},
  {0, 0, 0, 0, 0.000113487, 0.000567436, 0.000887871, 0.00167084, 0.00201225,
   0.000875473, 0.000717163, 0.000752449, 0.0007658},
  {0, 0, 0, 0, 0, 0.00111771, 0.00438976, 0.00933838, 0.00513363, 0.0140743,
   0.00801277, 0.0159092, 0.02176},
  {0, 0, 0, 0, 0, 0, 0.00159454, 0.00238228, 0.0140562, 0.01859, 0.0102701,
   0.0192928, 0.0259924},
  {0, 0, 0, 0, 0, 0, 0, 0.00282097, 0.00977421, 0.00655842, 0.0137911,
   0.0193491, 0.0249863},
  {0, 0, 0, 0, 0, 0, 0, 0, 0.00287819, 0.00488853, 0.00852299, 0.0253763,
   0.0242138},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00213337, 0.00696659, 0.0209045, 0.0477057},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00154686, 0.00809574, 0.0122766},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00313854, 0.0310097},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.00335312}};

ScoreDistributionTable::ScoreDistributionTable() : possible_min(0), possible_max(29), min_score(29), max_score(0) {
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

template <typename T>
Table<T>::Table() {
    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 13; ++j) {
            table_dealer_crib[i][j] = T();
            table_opp_crib[i][j] = T();
        }
    }
}

template <typename T>
Table<T>::Table(std::string stats_name) : stats_name(stats_name) {
    for (int i = 0; i < 13; ++i) {
        for (int j = 0; j < 13; ++j) {
            table_dealer_crib[i][j] = T();
            table_opp_crib[i][j] = T();
        }
    }
}

template <typename T>
void Table<T>::save(std::string filename) {
    std::ofstream file(filename);
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            file << table_dealer_crib[i][j] << " ";
        }
        file << std::endl;
    }
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            file << table_opp_crib[i][j] << " ";
        }
        file << std::endl;
    }
    file.close();
}

/*
 * Make pretty pritn function look like this:
 * <Name of Stat>
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 * My Crib
 * --------------------
 *     | 1  2  3  4  5  6  7  8  9 10 11 12 13
 *  ---|--------------------------------------
 *  1  | 2  0  1 32  1  2  3  4  5  6 26  7  8
 *  2  | 0  9  0  1 32  1  2  3  4  5  6 26  7
 *  3  | 1  0 29  0  1 32  1  2  3  4  5  6 26
 *  4  | 32 1  0  2  0  1 32  1  2  3  4  5  6
 *  5  | ETC ...
 *  6  | ...
 *  7  | ...
 *  8  |
 *  9  |
 *  10 |
 *  11 |
 *  12 |
 *  13 |
 *
 * Opponent's Crib
 * --------------------
 *     | 1  2  3  4  5  6  7  8  9 10 11 12 13
 *  ---|--------------------------------------
 *  1  | 2  0  1 32  1  2  3  4  5  6 26  7  8
 *  2  | 0  9  0  1 32  1  2  3  4  5  6 26  7
 *  3  | 1  0 29  0  1 32  1  2  3  4  5  6 26
 *  4  | 32 1  0  2  0  1 32  1  2  3  4  5  6
 *  5  | ETC ...
 *  6  | ...
 *  7  | ...
 *  8  |
 *  9  |
 *  10 |
 *  11 |
 *  12 |
 *  13 |
 *
 *
 *
 * It should work with floats and ints and
 * works with any number of digits
 */
template <typename T>
void Table<T>::pretty_print() {
    int width = 2;
    if (typeid(T) == typeid(float)) {
        width = 8;
    }
    std::cout << stats_name << std::endl;
    std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-"
                 "=-=-=-=-=-=-=-=-=-"
              << std::endl;
    std::cout << "My Crib" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "    |";
    for (int i = 0; i < 13; ++i) {
        std::cout << " " << std::setw(width) << i + 1;
    }
    std::cout << std::endl;
    std::cout << " ---|";
    for (int i = 0; i < 13; ++i) {
        std::cout << "---";
    }
    std::cout << std::endl;
    for (int i = 0; i < 13; ++i) {
        std::cout << " " << std::setw(width) << i + 1 << "  |";
        for (int j = 0; j < 13; ++j) {
            std::cout << " " << std::setw(width) << table_dealer_crib[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Opponent's Crib" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "    |";
    for (int i = 0; i < 13; ++i) {
        std::cout << " " << std::setw(width) << i + 1;
    }
    std::cout << std::endl;
    std::cout << " ---|";
    for (int i = 0; i < 13; ++i) {
        std::cout << "---";
    }
    std::cout << std::endl;
    for (int i = 0; i < 13; ++i) {
        std::cout << " " << std::setw(width) << i + 1 << "  |";
        for (int j = 0; j < 13; ++j) {
            std::cout << " " << std::setw(width) << table_opp_crib[i][j];
        }
        std::cout << std::endl;
    }
}

template <typename T>
int Table<T>::load(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 0;
    }
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            file >> table_dealer_crib[i][j];
        }
    }
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            file >> table_opp_crib[i][j];
        }
    }
    file.close();
    return 1;
}

CribDiscardProbabilities::CribDiscardProbabilities()
    : table("Crib Discard Probabilities"), num_sim_discards(0) {}

CribDiscardProbabilities::CribDiscardProbabilities(int num_sim_discards)
    : table("Crib Discard Probabilities"), num_sim_discards(num_sim_discards) {}

const float& CribDiscardProbabilities::get_prob(Card card1, Card card2,
                                                bool is_dealer) const {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    return get_table_value(index1, index2, is_dealer);
}

const float& CribDiscardProbabilities::get_prob(Rank rank1, Rank rank2,
                                                bool is_dealer) const {
    int index1 = static_cast<int>(rank1) - 1;
    int index2 = static_cast<int>(rank2) - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    return get_table_value(index1, index2, is_dealer);
}

const float& CribDiscardProbabilities::get_table_value(int index1, int index2,
                                                       bool is_dealer) const {
    if (is_dealer) {
        return table.get_dealer_crib(index1, index2);
    } else {
        return table.get_opp_crib(index1, index2);
    }
}

float& CribDiscardProbabilities::get_table_value(int index1, int index2,
                                                 bool is_dealer) {
    if (is_dealer) {
        return table.get_dealer_crib(index1, index2);
    } else {
        return table.get_opp_crib(index1, index2);
    }
}

float& CribDiscardProbabilities::get_table_value(Rank rank1, Rank rank2,
                                                 bool is_dealer) {
    int index1 = static_cast<int>(rank1) - 1;
    int index2 = static_cast<int>(rank2) - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    return get_table_value(index1, index2, is_dealer);
}

void CribDiscardProbabilities::to_freq_table() {
    if (is_freq_table) {
        return;
    }
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            table.get_dealer_crib(i, j) *= num_sim_discards;
            table.get_opp_crib(i, j) *= num_sim_discards;
        }
    }
    is_freq_table = true;
}

void CribDiscardProbabilities::to_prob_table() {
    if (!is_freq_table) {
        return;
    }
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            table.get_dealer_crib(i, j) /= num_sim_discards;
            table.get_opp_crib(i, j) /= num_sim_discards;
        }
    }
    is_freq_table = false;
}

DiscardStatistics::DiscardStatistics(
  Card discard1, Card discard2, bool is_dealer, PlayerInfo player_info)
  
    : discard1(discard1),
      discard2(discard2),
      player_info(player_info),
      is_dealer(is_dealer),
      score_dist_hand(),
      score_dist_crib(),
      score_dist_combined(-60, 60) {}

ScoreDistributionTable& DiscardStatistics::get_score_dist(
  ScoreType score_type) {
    switch (score_type) {
        case ScoreType::HAND:
            return score_dist_hand;
        case ScoreType::CRIB:
            return score_dist_crib;
        case ScoreType::COMBINED:
            return score_dist_combined;
        default:
            std::cerr << "Error: Invalid score type" << std::endl;
            exit(1);
    }
}

const ScoreDistributionTable& DiscardStatistics::get_score_dist(
  ScoreType score_type) const {
    switch (score_type) {
        case ScoreType::HAND:
            return score_dist_hand;
        case ScoreType::CRIB:
            return score_dist_crib;
        case ScoreType::COMBINED:
            return score_dist_combined;
        default:
            std::cerr << "Error: Invalid score type" << std::endl;
            exit(1);
    }
}

float DiscardStatistics::get_mean(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_mean();
}

int DiscardStatistics::get_median(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_median();
}

float DiscardStatistics::get_variance(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_variance();
}

float DiscardStatistics::get_std_dev(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_std_dev();
}

int DiscardStatistics::get_max(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_max();
}

int DiscardStatistics::get_min(ScoreType score_type) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.calc_min();
}

float DiscardStatistics::get_prob(ScoreType score_type, int score) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.prob_score(score);
}

float DiscardStatistics::get_prob_cummulative(ScoreType score_type,
                                              int score) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.prob_score_cummulative(score);
}

float DiscardStatistics::get_prob_between(ScoreType score_type, int score1,
                                          int score2) const {
    const ScoreDistributionTable& score_dist = get_score_dist(score_type);
    return score_dist.prob_score_between(score1, score2);
}

std::string DiscardStatistics::get_discard_string() const {
    std::string discard_string = "Discards: ";
    discard_string += discard1.to_string();
    discard_string += " ";
    discard_string += discard2.to_string();
    discard_string += "\n";

    discard_string += "Hand Stats> ";
    discard_string +=
      "Mean: " + std::to_string(get_mean(ScoreType::HAND)) + " ";
    discard_string += "Min: " + std::to_string(get_min(ScoreType::HAND)) + " ";
    discard_string += "Max: " + std::to_string(get_max(ScoreType::HAND)) + " ";
    discard_string += "\n";

    discard_string += "Crib Stats> ";
    discard_string +=
      "Mean: " + std::to_string(get_mean(ScoreType::CRIB)) + " ";
    discard_string += "Min: " + std::to_string(get_min(ScoreType::CRIB)) + " ";
    discard_string += "Max: " + std::to_string(get_max(ScoreType::CRIB)) + " ";
    discard_string += "\n";

    discard_string += "Combined Stats> ";
    discard_string +=
      "Mean: " + std::to_string(get_mean(ScoreType::COMBINED)) + " ";
    discard_string +=
      "Min: " + std::to_string(get_min(ScoreType::COMBINED)) + " ";
    discard_string +=
      "Max: " + std::to_string(get_max(ScoreType::COMBINED)) + " ";
    discard_string += "\n";
    return discard_string;
}

std::ostream& operator<<(std::ostream& os,
                         const DiscardStatistics& discard_stats) {
    os << discard_stats.get_discard_string();
    return os;
}

AllDiscardStatistics::AllDiscardStatistics() : discard_stats() {}

const DiscardStatistics& AllDiscardStatistics::get_discard_stats(Card discard1, Card discard2) const {
    for (auto it = discard_stats.cbegin(); it != discard_stats.cend(); ++it) {
        if ((*it).get_discard1() == discard1 &&
            (*it).get_discard2() == discard2) {
            return *it;
        }
    }
    std::cerr << "Error: Discard statistics not found" << std::endl;
    throw std::exception();
}

void AllDiscardStatistics::sort_discard_stats(ScoreType score_type, Statistic stat) {
    auto comparison_heuristic = [score_type](const DiscardStatistics& a,
                                             const DiscardStatistics& b) {
        return a.get_mean(score_type) > b.get_mean(score_type);
    };

    switch (stat) {
        case Statistic::MEAN:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             [score_type](const DiscardStatistics& a,
                                          const DiscardStatistics& b) {
                                 return a.get_mean(score_type) >
                                        b.get_mean(score_type);
                             });
            break;
        case Statistic::MEDIAN:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::VARIANCE:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::STD_DEV:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::MAX:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        case Statistic::MIN:
            std::stable_sort(discard_stats.begin(), discard_stats.end(),
                             comparison_heuristic);
            break;
        default:
            std::cerr << "Error: Invalid statistic" << std::endl;
            exit(1);
    }
}

const DiscardStatistics& AllDiscardStatistics::get_best_discard_stats() const {
    return discard_stats[0];
}

std::string AllDiscardStatistics::get_discard_stats_string(
  std::optional<int> num_discard_stats) const {
    int num = 15; // Default
    if (num_discard_stats)
        num = *num_discard_stats;

    std::string discard_stats_string = "";
    for (int i = 0; i < num; i++) {
        discard_stats_string += discard_stats[i].get_discard_string();
        discard_stats_string += "\n";
    }
    return discard_stats_string;
}

void AllDiscardStatistics::print_discard_stats(
  int num_discard_stats) const {
    std::cout << get_discard_stats_string(num_discard_stats);
}

std::ostream& operator<<(std::ostream& os,
                         const AllDiscardStatistics& gen_discard_stats) {
    for (auto it = gen_discard_stats.discard_stats.cbegin();
         it != gen_discard_stats.discard_stats.cend(); ++it) {
        os << *it << std::endl;
    }
    return os;
}

StatisticTable::StatisticTable()
    : prob_table(),
      score_dist_table("Score Distribution Table"),
      mean_table("Mean Table"),
      median_table("Median Table"),
      variance_table("Variance Table"),
      std_dev_table("Standard Deviation Table"),
      max_table("Max Table"),
      min_table("Min Table") {}

void StatisticTable::load_default_tables() {
    for (int i = 0; i < 13; ++i) {
        for (int j = i; j < 13; ++j) {
            prob_table.table.get_dealer_crib(i, j) =
              default_stat_vs_stat_my_crib[i][j];
            prob_table.table.get_opp_crib(i, j) =
              default_stat_vs_stat_opp_crib[i][j];
        }
    }
    prob_table.num_sim_discards = default_num_sim_discards;
}

int StatisticTable::load_tables(std::optional<std::string> table) {
    // TODO: If no dirname, then load default tables
    if (!table) {
        load_default_tables();
        return 1;
    }
    std::string dirname_string = TABLE_DIR + table.value();

    int num_loaded = 0;
    num_loaded += prob_table.table.load(dirname_string + "freq_table.txt");
    num_loaded += mean_table.load(dirname_string + "mean_table.txt");
    num_loaded += median_table.load(dirname_string + "median_table.txt");
    num_loaded += variance_table.load(dirname_string + "variance_table.txt");
    num_loaded += std_dev_table.load(dirname_string + "std_dev_table.txt");
    num_loaded += max_table.load(dirname_string + "max_table.txt");
    num_loaded += min_table.load(dirname_string + "min_table.txt");
    return num_loaded;
}

void StatisticTable::save_tables(std::string table) {
    prob_table.table.save(table + "freq_table.txt");
    // TODO: save score_dist_table
    mean_table.save(table + "mean_table.txt");
    median_table.save(table + "median_table.txt");
    variance_table.save(table + "variance_table.txt");
    std_dev_table.save(table + "std_dev_table.txt");
    max_table.save(table + "max_table.txt");
    min_table.save(table + "min_table.txt");
}

ScoreDistributionTable StatisticTable::get_score_dist(Card card1, Card card2,
                                                      bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return score_dist_table.get_dealer_crib(index1, index2);
    } else {
        return score_dist_table.get_opp_crib(index1, index2);
    }
}

float StatisticTable::get_mean(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return mean_table.get_dealer_crib(index1, index2);
    } else {
        return mean_table.get_opp_crib(index1, index2);
    }
}

int StatisticTable::get_median(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return median_table.get_dealer_crib(index1, index2);
    } else {
        return median_table.get_opp_crib(index1, index2);
    }
}

float StatisticTable::get_variance(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return variance_table.get_dealer_crib(index1, index2);
    } else {
        return variance_table.get_opp_crib(index1, index2);
    }
}

float StatisticTable::get_std_dev(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return std_dev_table.get_dealer_crib(index1, index2);
    } else {
        return std_dev_table.get_opp_crib(index1, index2);
    }
}

int StatisticTable::get_max(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return max_table.get_dealer_crib(index1, index2);
    } else {
        return max_table.get_opp_crib(index1, index2);
    }
}

int StatisticTable::get_min(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return min_table.get_dealer_crib(index1, index2);
    } else {
        return min_table.get_opp_crib(index1, index2);
    }
}

}  // namespace cribbage
