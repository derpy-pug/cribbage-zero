#include "generate_crib_statistics.h"
#include "deck.h"

#include <fstream>

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
    : freq_table(), freq_num_games(0),
    /* score_freq_table_my_crib(), score_freq_table_opp_crib(), */ 
    score_dist_table(), freq_tables_generated(false), 
    mean_table(),
    max_table(), min_table(),
    dealer(dealer), pone(pone)
{
}

void GenerateCribStatistics::generate_mean_tables()
{
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
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
            int min = 29;
            for (int k = 0; k < 30; k++)
            {
                if (current_score_dist_table_my_crib.dist_table[k] > 0) {
                    min = k;
                    break;
                }
            }
            for (int k = 30; k >= 0; k--)
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
            min = 29;
            for (int k = 0; k < 29; k++)
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
