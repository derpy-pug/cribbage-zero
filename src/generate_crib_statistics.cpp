#include "generate_crib_statistics.h"
#include "deck.h"

#include <fstream>
#include <iomanip>
#include <math.h>

#include "scoring.h"

template <typename T>
Table<T>::Table() {
    for (int i = 0; i < 13; ++i)
    {
        for (int j = 0; j < 13; ++j)
        {
            table_my_crib[i][j] = T();
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
            table_my_crib[i][j] = T();
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
            file << table_my_crib[i][j] << " ";
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
    std::cout << stats_name << std::endl;
    std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
    std::cout << "My Crib" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "    |";
    for (int i = 0; i < 13; ++i)
    {
        std::cout << " " << std::setw(8) << i + 1;
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
        std::cout << " " << std::setw(8) << i + 1 << "  |";
        for (int j = 0; j < 13; ++j)
        {
            std::cout << " " << std::setw(8) << table_my_crib[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    std::cout << "Opponent's Crib" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "    |";
    for (int i = 0; i < 13; ++i)
    {
        std::cout << " " << std::setw(8) << i + 1;
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
        std::cout << " " << std::setw(8) << i + 1 << "  |";
        for (int j = 0; j < 13; ++j)
        {
            std::cout << " " << std::setw(8) << table_opp_crib[i][j];
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
            file >> table_my_crib[i][j];
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


GenerateCribStatistics::GenerateCribStatistics(Player* dealer, Player* pone)
    : freq_table("Freq"), freq_num_games(0),
    /* score_freq_table_my_crib(), score_freq_table_opp_crib(), */ 
    score_dist_table("Distribution"), freq_tables_generated(false), 
    mean_table("Means"), mean_tables_generated(false),
    median_table("Medians"),
    variance_table("Variances"), variance_tables_generated(false),
    std_dev_table("Standard Deviations"),
    max_table("Maxs"), min_table("Mins"),
    dealer(dealer), pone(pone)
{
}

void GenerateCribStatistics::pretty_print_freq_tables()
{
    freq_table.pretty_print();
}

void GenerateCribStatistics::pretty_print_max_tables()
{
    max_table.pretty_print();
}

void GenerateCribStatistics::generate_all_tables()
{
    generate_freq_tables();
    generate_max_min_tables();
    generate_mean_tables();
    generate_median_tables();
    generate_variance_tables();
    generate_std_dev_tables();
}

void GenerateCribStatistics::generate_mean_tables()
{
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
    freq_tables_generated = true;
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreDistributionTable current_score_dist_table_my_crib = score_dist_table.get_my_crib(i, j);
            float mean = 0;
            for (int k = 0; k < 30; k++)
            {
                mean += k * current_score_dist_table_my_crib.dist_table[k];
            }
            mean_table.get_my_crib(i, j) = mean;

            ScoreDistributionTable current_score_dist_table_opp_crib = score_dist_table.get_opp_crib(i, j);
            mean = 0;
            for (int k = 0; k < 30; k++)
            {
                mean += k * current_score_dist_table_opp_crib.dist_table[k];
            }
            mean_table.get_opp_crib(i, j) = mean;
        }
    } 
}

void GenerateCribStatistics::generate_median_tables()
{
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreDistributionTable current_score_dist_table_my_crib = score_dist_table.get_my_crib(i, j);
            float median = 0;
            float sum = 0;
            for (int k = 0; k < 30; k++)
            {
                sum += current_score_dist_table_my_crib.dist_table[k];
                if (sum >= 0.5) {
                    median = k;
                    break;
                }
            }
            median_table.get_my_crib(i, j) = median;

            ScoreDistributionTable current_score_dist_table_opp_crib = score_dist_table.get_opp_crib(i, j);
            median = 0;
            sum = 0;
            for (int k = 0; k < 30; k++)
            {
                sum += current_score_dist_table_opp_crib.dist_table[k];
                if (sum >= 0.5) {
                    median = k;
                    break;
                }
            }
            median_table.get_opp_crib(i, j) = median;
        }
    } 
}

void GenerateCribStatistics::generate_variance_tables()
{
    if (!mean_tables_generated) {
        generate_mean_tables();
    }
    variance_tables_generated = true;
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreDistributionTable current_score_dist_table_my_crib = score_dist_table.get_my_crib(i, j);
            float mean = mean_table.get_my_crib(i, j);
            float std_dev = 0;
            for (int k = 0; k < 30; k++)
            {
                std_dev += (k - mean) * (k - mean) * current_score_dist_table_my_crib.dist_table[k];
            }
            variance_table.get_my_crib(i, j) = std_dev;

            ScoreDistributionTable current_score_dist_table_opp_crib = score_dist_table.get_opp_crib(i, j);
            mean = mean_table.get_opp_crib(i, j);
            std_dev = 0;
            for (int k = 0; k < 30; k++)
            {
                std_dev += (k - mean) * (k - mean) * current_score_dist_table_opp_crib.dist_table[k];
            }
            variance_table.get_opp_crib(i, j) = std_dev;
        }
    } 
}

void GenerateCribStatistics::generate_std_dev_tables()
{
    if (!variance_tables_generated) {
        generate_variance_tables();
    }
    variance_tables_generated = true;
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreDistributionTable current_score_dist_table_my_crib = score_dist_table.get_my_crib(i, j);
            float variance = variance_table.get_my_crib(i, j);
            std_dev_table.get_my_crib(i, j) = sqrt(variance);

            ScoreDistributionTable current_score_dist_table_opp_crib = score_dist_table.get_opp_crib(i, j);
            variance = variance_table.get_opp_crib(i, j);
            std_dev_table.get_opp_crib(i, j) = sqrt(variance);
        }
    } 
}

void GenerateCribStatistics::generate_max_min_tables() {
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreDistributionTable current_score_dist_table_my_crib = score_dist_table.get_my_crib(i, j);
            int max = 0;
            int min = 0;
            for (int k = 0; k <= 29; k++)
            {
                if (current_score_dist_table_my_crib.dist_table[k] > 0) {
                    min = k;
                    break;
                }
            }
            for (int k = 29; k >= 0; k--)
            {
                if (current_score_dist_table_my_crib.dist_table[k] > 0) {
                    max = k;
                    break;
                }
            }
            max_table.get_my_crib(i, j) = max;
            min_table.get_my_crib(i, j) = min;

            ScoreDistributionTable current_score_dist_table_opp_crib = score_dist_table.get_opp_crib(i, j);
            max = 0;
            min = 0;
            for (int k = 0; k <= 29; k++)
            {
                if (current_score_dist_table_opp_crib.dist_table[k] > 0) {
                    min = k;
                    break;
                }
            }
            for (int k = 29; k >= 0; k--)
            {
                if (current_score_dist_table_opp_crib.dist_table[k] > 0) {
                    max = k;
                    break;
                }
            }
            max_table.get_opp_crib(i, j) = max;
            min_table.get_opp_crib(i, j) = min;
        }
    }
}

void GenerateCribStatistics::generate_freq_tables()
{
    generate_unscored_freq_tables();
    generate_scored_freq_tables();
    /* generate_score_dist_tables(); */
    freq_tables_generated = true;
}

void GenerateCribStatistics::generate_unscored_freq_tables()
{
    Deck deck;
    int total_games = 1000000;
    for (int i = 0; i < total_games; i++)
    {
        // TODO: Keep track of suits??

        deck.shuffleTopCardsIntoDeck(12);
        Hand dealer_hand = deck.deal_hand(6);
        dealer->set_hand(dealer_hand);
        Hand discards = dealer->make_discards(true);
        int freq_i = discards[0].get_rank_int() - 1;
        int freq_j = discards[1].get_rank_int() - 1;
        if (freq_i > freq_j) {
            std::swap(freq_i, freq_j);
        }
        freq_table.get_my_crib(freq_i, freq_j)++;

        Hand pone_hand = deck.deal_hand(6);
        pone->set_hand(pone_hand);
        Hand pone_discards = pone->make_discards(false);
        freq_i = pone_discards[0].get_rank_int() - 1;
        freq_j = pone_discards[1].get_rank_int() - 1;
        if (freq_i > freq_j) {
            std::swap(freq_i, freq_j);
        }
        freq_table.get_opp_crib(freq_i, freq_j)++;
    }
    freq_num_games += total_games;
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            freq_table.get_my_crib(i, j) /= freq_num_games;
            freq_table.get_opp_crib(i, j) /= freq_num_games;
        }
    }
}

void GenerateCribStatistics::generate_scored_freq_tables()
{
    const Deck deck; // For cut card
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreDistributionTable current_score_freq_table_my_crib;
            // For my crib
            for (int k = 0; k < 13; k++)
            {
                for (int l = k; l < 13; l++)
                {
                    for (auto it = deck.cbegin(); it != deck.cbegin() + 13; ++it) {
                        Card cut = *it;
                        int cut_rank_int = cut.get_rank_int() - 1;
                        if (cut_rank_int == i && i == j && j == k && k == l) {
                            continue;
                        }

                        int cut_num = 4;
                        if (cut_rank_int == i) cut_num--;
                        if (cut_rank_int == j) cut_num--;
                        if (cut_rank_int == k) cut_num--;
                        if (cut_rank_int == l) cut_num--;
                        float cut_prob = cut_num / 50.0;

                        float knob_prob = 0;
                        if (Rank(i + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }
                        if (Rank(j + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }
                        if (Rank(k + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }
                        if (Rank(l + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }


                        Hand hand;
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(i + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(j + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(k + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(l + 1)));
                        int score = score_hand_suitless(hand, cut);
                        current_score_freq_table_my_crib[score] += (1 - knob_prob) * cut_prob * freq_table.get_my_crib(k, l);
                        current_score_freq_table_my_crib[score + 1] += (knob_prob) * cut_prob * freq_table.get_my_crib(k, l);
                    }
                }
            }
            score_dist_table.get_my_crib(i, j) = current_score_freq_table_my_crib;


            ScoreDistributionTable current_score_freq_table_opp_crib;
            // For opponent's crib
            for (int k = 0; k < 13; k++)
            {
                for (int l = k; l < 13; l++)
                {
                    for (auto it = deck.cbegin(); it != deck.cbegin() + 13; ++it) {
                        Card cut = *it;
                        int cut_rank_int = cut.get_rank_int() - 1;
                        if (cut_rank_int == i && i == j && j == k && k == l) {
                            continue;
                        }

                        int cut_num = 4;
                        if (cut_rank_int == i) cut_num--;
                        if (cut_rank_int == j) cut_num--;
                        if (cut_rank_int == k) cut_num--;
                        if (cut_rank_int == l) cut_num--;
                        float cut_prob = cut_num / 50.0;

                        float knob_prob = 0;
                        if (Rank(i + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }
                        if (Rank(j + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }
                        if (Rank(k + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }
                        if (Rank(l + 1) == Rank::JACK) {
                            knob_prob += 12.0 / 50.0;
                        }

                        Hand hand;
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(i + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(j + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(k + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(l + 1)));
                        int score = score_hand_suitless(hand, cut);
                        current_score_freq_table_opp_crib[score] += (1 - knob_prob) * cut_prob * freq_table.get_opp_crib(k, l);
                        current_score_freq_table_opp_crib[score + 1] += (knob_prob) * cut_prob * freq_table.get_opp_crib(k, l);
                    }
                }
            }
            score_dist_table.get_opp_crib(i, j) = current_score_freq_table_opp_crib;
        }
    }
}

/* void GenerateStatistics::generate_score_dist_tables() { */ 
/*     for (int i = 0; i < 13; i++) { */
/*         for (int j = i; j < 13; ++j) { */
/*             ScoreFreqTable current_score_freq_table_my_crib = score_freq_table_my_crib[i][j]; */
/*             ScoreDistributionTable current_score_dist_table_my_crib; */
/*             for (int k = 0; k < 28; ++k) { */
/*                 int score = k; */
/*                 current_score_dist_table_my_crib.dist_table[score] = */
/*                     (float)current_score_freq_table_my_crib.freq_table[score] / current_score_freq_table_my_crib.num_games; */
/*             } */
/*             score_dist_table_my_crib[i][j] = current_score_dist_table_my_crib; */

/*             ScoreFreqTable current_score_freq_table_opp_crib = score_freq_table_opp_crib[i][j]; */
/*             ScoreDistributionTable current_score_dist_table_opp_crib; */
/*             for (int k = 0; k < 28; ++k) { */
/*                 int score = k; */
/*                 current_score_dist_table_opp_crib.dist_table[score] = */
/*                     (float)current_score_freq_table_opp_crib.freq_table[score] / current_score_freq_table_opp_crib.num_games; */
/*             } */
/*             score_dist_table_opp_crib[i][j] = current_score_dist_table_opp_crib; */

/*         } */
/*     } */
/* } */

void GenerateCribStatistics::save_mean_tables(std::string filename)
{
    mean_table.save(filename);
}

void GenerateCribStatistics::load_mean_tables(std::string filename)
{
    mean_table.load(filename);
}

void GenerateCribStatistics::save_median_tables(std::string filename)
{
    median_table.save(filename);
}

void GenerateCribStatistics::load_median_tables(std::string filename)
{
    median_table.load(filename);
}

void GenerateCribStatistics::save_freq_tables(std::string filename) 
{
    freq_table.save(filename);
}

void GenerateCribStatistics::load_freq_tables(std::string filename) 
{
    freq_table.load(filename);
}

void GenerateCribStatistics::save_max_min_tables(std::string filename) 
{
    max_table.save("max_" + filename);
    min_table.save("min_" + filename);
}

void GenerateCribStatistics::load_max_min_tables(std::string filename) 
{
    max_table.load("max_" + filename);
    min_table.load("min_" + filename);
}

void GenerateCribStatistics::save_variance_tables(std::string filename) 
{
    variance_table.save(filename);
}

void GenerateCribStatistics::load_variance_tables(std::string filename) 
{
    variance_table.load(filename);
}

void GenerateCribStatistics::save_std_dev_tables(std::string filename) 
{
    std_dev_table.save(filename);
}

void GenerateCribStatistics::load_std_dev_tables(std::string filename) 
{
    std_dev_table.load(filename);
}
