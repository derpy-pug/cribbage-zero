#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>


namespace cribbage {

/*
 * @brief Table for storing the number of times a score occurs.
 */
class ScoreDistributionTable {
  public:
    ScoreDistributionTable();
    ScoreDistributionTable(int min, int max);

    float calc_mean() const;
    int calc_median() const;
    float calc_variance() const;
    float calc_std_dev() const;
    int calc_max() const;
    int calc_min() const;

    float prob_score(int score) const;
    float prob_score_cummulative(int score) const;
    float prob_score_between(int score1, int score2) const;

    float& operator[](int score);
    const float& operator[](int score) const;

    int get_possible_score_min() const { return possible_min; }
    int get_possible_score_max() const { return possible_max; }

    int get_score_min() const { return min_score; }
    int get_score_max() const { return max_score; }

    void clear();

    void normalize();

  private:
    float& get_table_value(int score);
    const float& get_table_value(int score) const;

  private:
    std::vector<float>
      dist_table;  // 0-29 for single hand, -60 through 60 for combined
    int possible_min;
    int possible_max;
    int min_score;
    int max_score;
};

template <typename T>
class Table {
  public:
    /*
     * @brief initializes all values to default.
     */
    Table();

    /*
     * @brief Constructor with stat name used for printing.
     */
    Table(std::string stats_name);

    T& get_dealer_crib(int index1, int index2) {
        return table_dealer_crib[index1][index2];
    }
    const T& get_dealer_crib(int index1, int index2) const {
        return table_dealer_crib[index1][index2];
    }
    T& get_opp_crib(int index1, int index2) {
        return table_opp_crib[index1][index2];
    }
    const T& get_opp_crib(int index1, int index2) const {
        return table_opp_crib[index1][index2];
    }

    void pretty_print();

    void save(std::string filename);
    int load(std::string filename);

  private:
    std::string stats_name;
    T table_dealer_crib[13][13];
    T table_opp_crib[13][13];
};

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
} // namespace cribbage
