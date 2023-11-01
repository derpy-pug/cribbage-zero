#include "generate_statistics.h"
#include "deck.h"

#include <fstream>

#include "scoring.h"

GenerateStatistics::GenerateStatistics(Player* dealer, Player* pone)
    : freq_table_my_crib(), freq_table_opp_crib(), num_games(0), 
    score_freq_table_my_crib(), score_freq_table_opp_crib(), 
    score_dist_table_my_crib(), score_dist_table_opp_crib(), freq_tables_generated(false), 
    mean_table_my_crib(), mean_table_opp_crib(), max_table(), min_table(), dealer(dealer), pone(pone)
{
}

void GenerateStatistics::generate_mean_tables()
{
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreDistributionTable current_score_dist_table_my_crib = score_dist_table_my_crib[i][j];
            float mean = 0;
            for (int k = 0; k < 29; k++)
            {
                mean += k * current_score_dist_table_my_crib.dist_table[k];
            }
            mean_table_my_crib[i][j] = mean;

            ScoreDistributionTable current_score_dist_table_opp_crib = score_dist_table_opp_crib[i][j];
            mean = 0;
            for (int k = 0; k < 29; k++)
            {
                mean += k * current_score_dist_table_opp_crib.dist_table[k];
            }
            mean_table_opp_crib[i][j] = mean;
        }
    } 
}

void GenerateStatistics::generate_freq_tables()
{
    generate_unscored_freq_tables();
    generate_scored_freq_tables();
    generate_score_dist_tables();
    freq_tables_generated = true;
}

void GenerateStatistics::generate_unscored_freq_tables()
{
    Deck deck;
    int total_games = 1000000;
    for (int i = 0; i < total_games; i++)
    {
        // TODO: Keep track of suits??

        deck.shuffle();
        Hand dealer_hand = deck.deal_hand(6);
        dealer->set_hand(dealer_hand);
        Hand discards = dealer->make_discards(true);
        int freq_i = discards[0].get_rank_int() - 1;
        int freq_j = discards[1].get_rank_int() - 1;
        if (freq_i > freq_j) {
            std::swap(freq_i, freq_j);
        }
        freq_table_my_crib[freq_i][freq_j]++;

        Hand pone_hand = deck.deal_hand(6);
        pone->set_hand(pone_hand);
        Hand pone_discards = pone->make_discards(false);
        freq_i = pone_discards[0].get_rank_int() - 1;
        freq_j = pone_discards[1].get_rank_int() - 1;
        if (freq_i > freq_j) {
            std::swap(freq_i, freq_j);
        }
        freq_table_opp_crib[freq_i][freq_j]++;
    }
    num_games += total_games;
}

void GenerateStatistics::generate_scored_freq_tables()
{
    const Deck deck; // For cut card
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            ScoreFreqTable current_score_freq_table_my_crib;
            // For my crib
            for (int k = 0; k < 13; k++)
            {
                for (int l = k; l < 13; l++)
                {
                    int num_cards = 13;
                    for (auto it = deck.cbegin(); it != deck.cbegin() + 13; ++it) {
                        Card card = *it;
                        if (card.get_rank_int() - 1 == i && i == j && j == k && k == l) {
                            num_cards--;
                            continue;
                        }
                        Hand hand;
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(i + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(j + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(k + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(l + 1)));
                        int score = score_hand_suitless(hand, card);
                        current_score_freq_table_my_crib.freq_table[score]++;
                    }
                    current_score_freq_table_my_crib.num_games += num_cards;
                }
            }
            score_freq_table_my_crib[i][j] = current_score_freq_table_my_crib;


            ScoreFreqTable current_score_freq_table_opp_crib;
            // For opponent's crib
            for (int k = 0; k < 13; k++)
            {
                for (int l = k; l < 13; l++)
                {
                    int num_cards = 13;
                    for (auto it = deck.begin(); it != deck.begin() + 13; ++it) {
                        Card card = *it;
                        if (card.get_rank_int() - 1 == i && i == j && j == k && k == l) {
                            num_cards--;
                            continue;
                        }
                        Hand hand;
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(i + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(j + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(k + 1)));
                        hand.add_card(Card(static_cast<Suit>(0), static_cast<Rank>(l + 1)));
                        int score = score_hand_suitless(hand, card);
                        current_score_freq_table_my_crib.freq_table[score]++;
                    }
                    current_score_freq_table_my_crib.num_games += num_cards;
                }
            }
        }
    }
}

void GenerateStatistics::generate_score_dist_tables() { 
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; ++j) {
            ScoreFreqTable current_score_freq_table_my_crib = score_freq_table_my_crib[i][j];
            ScoreDistributionTable current_score_dist_table_my_crib;
            for (int k = 0; k < 29; ++k) {
                int score = k;
                current_score_dist_table_my_crib.dist_table[score] =
                    (float)current_score_freq_table_my_crib.freq_table[score] / current_score_freq_table_my_crib.num_games;
            }

            ScoreFreqTable current_score_freq_table_opp_crib = score_freq_table_opp_crib[i][j];
            ScoreDistributionTable current_score_dist_table_opp_crib;
            for (int k = 0; k < 29; ++k) {
                int score = k;
                current_score_dist_table_opp_crib.dist_table[score] =
                    (float)current_score_freq_table_opp_crib.freq_table[score] / current_score_freq_table_opp_crib.num_games;
            }

        }
    }
}

void GenerateStatistics::save_mean_tables(std::string filename)
{
    std::ofstream file(filename);
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            file << mean_table_my_crib[i][j] << " ";
        }
        file << std::endl;
    }
    for (int i = 0; i < 13; i++)
    {
        for (int j = i; j < 13; j++)
        {
            file << mean_table_opp_crib[i][j] << " ";
        }
        file << std::endl;
    }
    file.close();
}
