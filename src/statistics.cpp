#include "statistics.h"

#include <fstream>
#include <iomanip>
#include <iostream>

template <typename T>
Table<T>::Table() {
    for (int i = 0; i < 13; ++i)
    {
        for (int j = 0; j < 13; ++j)
        {
            table_dealer_crib[i][j] = T();
            table_opp_crib[i][j] = T();
        }
    }
}

template <typename T>
Table<T>::Table(std::string stats_name)
    : stats_name(stats_name)
{
    for (int i = 0; i < 13; ++i)
    {
        for (int j = 0; j < 13; ++j)
        {
            table_dealer_crib[i][j] = T();
            table_opp_crib[i][j] = T();
        }
    }
}

template <typename T>
void Table<T>::save(std::string filename) {
    std::ofstream file(filename);
    for (int i = 0; i < 13; ++i)
    {
        for (int j = i; j < 13; ++j)
        {
            file << table_dealer_crib[i][j] << " ";
        }
        file << std::endl;
    }
    for (int i = 0; i < 13; ++i)
    {
        for (int j = i; j < 13; ++j)
        {
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
    std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
    std::cout << "My Crib" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "    |";
    for (int i = 0; i < 13; ++i)
    {
        std::cout << " " << std::setw(width) << i + 1;
    }
    std::cout << std::endl;
    std::cout << " ---|";
    for (int i = 0; i < 13; ++i)
    {
        std::cout << "---";
    }
    std::cout << std::endl;
    for (int i = 0; i < 13; ++i)
    {
        std::cout << " " << std::setw(width) << i + 1 << "  |";
        for (int j = 0; j < 13; ++j)
        {
            std::cout << " " << std::setw(width) << table_dealer_crib[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Opponent's Crib" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "    |";
    for (int i = 0; i < 13; ++i)
    {
        std::cout << " " << std::setw(width) << i + 1;
    }
    std::cout << std::endl;
    std::cout << " ---|";
    for (int i = 0; i < 13; ++i)
    {
        std::cout << "---";
    }
    std::cout << std::endl;
    for (int i = 0; i < 13; ++i)
    {
        std::cout << " " << std::setw(width) << i + 1 << "  |";
        for (int j = 0; j < 13; ++j)
        {
            std::cout << " " << std::setw(width) << table_opp_crib[i][j];
        }
        std::cout << std::endl;
    }
}

template <typename T>
void Table<T>::load(std::string filename) {
    std::ifstream file(filename);    
    for (int i = 0; i < 13; ++i)
    {
        for (int j = i; j < 13; ++j)
        {
            file >> table_dealer_crib[i][j];
        }
    }
    for (int i = 0; i < 13; ++i)
    {
        for (int j = i; j < 13; ++j)
        {
            file >> table_opp_crib[i][j];
        }
    }
    file.close();
}

Statistic::Statistic()
    : freq_table("Frequency Table"), freq_num_games(0),
      score_dist_table("Score Distribution Table"), freq_tables_generated(false),
      mean_table("Mean Table"), mean_tables_generated(false),
      median_table("Median Table"),
      variance_table("Variance Table"), variance_tables_generated(false),
      std_dev_table("Standard Deviation Table"),
      max_table("Max Table"),
      min_table("Min Table")
{
}

void Statistic::load_tables(std::string dirname) {
    freq_table.load(dirname + "freq_table.txt");
    // TODO: load score_dist_table
    mean_table.load(dirname + "mean_table.txt");
    median_table.load(dirname + "median_table.txt");
    variance_table.load(dirname + "variance_table.txt");
    std_dev_table.load(dirname + "std_dev_table.txt");
    max_table.load(dirname + "max_table.txt");
    min_table.load(dirname + "min_table.txt");
}

void Statistic::save_tables(std::string dirname) {
    freq_table.save(dirname + "freq_table.txt");
    // TODO: save score_dist_table
    mean_table.save(dirname + "mean_table.txt");
    median_table.save(dirname + "median_table.txt");
    variance_table.save(dirname + "variance_table.txt");
    std_dev_table.save(dirname + "std_dev_table.txt");
    max_table.save(dirname + "max_table.txt");
    min_table.save(dirname + "min_table.txt");
}

int Statistic::get_freq(Card card1, Card card2, bool is_dealer) {
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

ScoreDistributionTable Statistic::get_score_dist(Card card1, Card card2, bool is_dealer) {
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

float Statistic::get_mean(Card card1, Card card2, bool is_dealer) {
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

int Statistic::get_median(Card card1, Card card2, bool is_dealer) {
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

float Statistic::get_variance(Card card1, Card card2, bool is_dealer) {
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

float Statistic::get_std_dev(Card card1, Card card2, bool is_dealer) {
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

int Statistic::get_max(Card card1, Card card2, bool is_dealer) {
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

int Statistic::get_min(Card card1, Card card2, bool is_dealer) {
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
