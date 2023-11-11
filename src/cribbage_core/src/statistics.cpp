#include "statistics.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>

ScoreDistributionTable::ScoreDistributionTable() : min(0), max(29) {
    dist_table.resize(30);
}

ScoreDistributionTable::ScoreDistributionTable(int min, int max)
    : min(min), max(max) {
    dist_table.resize(max - min + 1);
}

float& ScoreDistributionTable::get_table_value(int score) {
    if (score < min || score > max) {
        std::cerr << "Score out of range: " << score << std::endl;
        exit(1);
    }
    int score_index = score - min;
    return dist_table[score_index];
}

const float& ScoreDistributionTable::get_table_value(int score) const {
    if (score < min || score > max) {
        std::cerr << "Score out of range: " << score << std::endl;
        exit(1);
    }
    int score_index = score - min;
    return dist_table[score_index];
}

float ScoreDistributionTable::calc_mean() const {
    float mean = 0;
    for (int i = min; i < max; ++i) {
        mean += i * get_table_value(i);
    }
    return mean;
}

int ScoreDistributionTable::calc_median() const {
    float cummulative = 0;
    for (int i = min; i < max; ++i) {
        cummulative += get_table_value(i);
        if (cummulative >= 0.5) {
            return i;
        }
    }
    return 0;
}

int ScoreDistributionTable::calc_max() const {
    for (int i = max; i >= min; --i) {
        if (get_table_value(i) > 0) {
            return i;
        }
    }
    return 0;
}

int ScoreDistributionTable::calc_min() const {
    for (int i = min; i < max; ++i) {
        if (get_table_value(i) > 0) {
            return i;
        }
    }
    return 0;
}

float ScoreDistributionTable::calc_variance() const {
    float mean = calc_mean();
    float variance = 0;
    for (int i = min; i < max; ++i) {
        variance += (i - mean) * (i - mean) * get_table_value(i);
    }
    return variance;
}

float ScoreDistributionTable::calc_std_dev() const {
    return std::sqrt(calc_variance());
}

float ScoreDistributionTable::prob_score(int score) const {
    if (score < min || score > max) {
        return 0;
    }
    return get_table_value(score);
}

float ScoreDistributionTable::prob_score_cummulative(int score) const {
    if (score < min) {
        return 0;
    }
    if (score > max) {
        return 1;
    }
    float cummulative = 0;
    for (int i = min; i <= score; ++i) {
        cummulative += get_table_value(i);
    }
    return cummulative;
}

float ScoreDistributionTable::prob_score_between(int score1, int score2) const {
    if (score1 > score2) {
        std::swap(score1, score2);
    }
    if (score1 < min) {
        score1 = min;
    }
    if (score2 > max) {
        score2 = max;
    }
    float cummulative = 0;
    for (int i = score1; i <= score2; ++i) {
        cummulative += get_table_value(i);
    }
    return cummulative;
}

float& ScoreDistributionTable::operator[](int score) {
    if (score < min || score > max) {
        std::cerr << "Score out of range: " << score << std::endl;
        exit(1);
    }
    return get_table_value(score);
}

const float& ScoreDistributionTable::operator[](int score) const {
    if (score < min || score > max) {
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

StatisticTable::StatisticTable()
    : freq_table("Frequency Table"),
      freq_num_games(0),
      score_dist_table("Score Distribution Table"),
      freq_tables_generated(false),
      mean_table("Mean Table"),
      mean_tables_generated(false),
      median_table("Median Table"),
      variance_table("Variance Table"),
      variance_tables_generated(false),
      std_dev_table("Standard Deviation Table"),
      max_table("Max Table"),
      min_table("Min Table") {}

int StatisticTable::load_tables(std::string dirname) {
    int num_loaded = 0;
    num_loaded += freq_table.load(dirname + "freq_table.txt");
    // TODO: load score_dist_table
    num_loaded += mean_table.load(dirname + "mean_table.txt");
    num_loaded += median_table.load(dirname + "median_table.txt");
    num_loaded += variance_table.load(dirname + "variance_table.txt");
    num_loaded += std_dev_table.load(dirname + "std_dev_table.txt");
    num_loaded += max_table.load(dirname + "max_table.txt");
    num_loaded += min_table.load(dirname + "min_table.txt");
    return num_loaded;
}

void StatisticTable::save_tables(std::string dirname) {
    freq_table.save(dirname + "freq_table.txt");
    // TODO: save score_dist_table
    mean_table.save(dirname + "mean_table.txt");
    median_table.save(dirname + "median_table.txt");
    variance_table.save(dirname + "variance_table.txt");
    std_dev_table.save(dirname + "std_dev_table.txt");
    max_table.save(dirname + "max_table.txt");
    min_table.save(dirname + "min_table.txt");
}

float StatisticTable::get_freq(Card card1, Card card2, bool is_dealer) {
    int index1 = card1.get_rank_int() - 1;
    int index2 = card2.get_rank_int() - 1;
    if (index1 > index2) {
        std::swap(index1, index2);
    }
    if (is_dealer) {
        return freq_table.get_dealer_crib(index1, index2);
    } else {
        return freq_table.get_opp_crib(index1, index2);
    }
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
