#include "statistics/generator/Crib.h"

#include <cmath>

#include "deck.h"
#include "player.h"
#include "scoring.h"

namespace cribbage {

GenerateCribStatistics::GenerateCribStatistics(Player* dealer, Player* pone)
    : dealer(dealer), pone(pone) {}

float GenerateCribStatistics::get_mean_counting_flush(Card card1, Card card2,
                                                      bool is_dealer,
                                                      int num_flush_cards) {
    float extra_flush = 0;
    if (card1.get_suit() == card2.get_suit()) {
        // Expected value of a flush given 2 cards of the same suit
        // and how many cards of that suit are in their remaining hand
        // This assumes that the oppenents discards suits are random
        // 0 in hand: 0.108696 * 5 = 0.054348
        // 1 in hand: 0.007905 * 5 = 0.039526
        // 2 in hand: 0.005533 * 5 = 0.027668
        // 3 in hand: 0.003689 * 5 = 0.018445
        // 4 in hand: 0.002306 * 5 = 0.011528
        static const float flush_probs[5] = {0.054348, 0.039526, 0.027668,
                                             0.018445, 0.011528};
        extra_flush = flush_probs[num_flush_cards];
    }
    if (is_dealer) {
        return stat_table.mean_table.get_dealer_crib(card1.get_rank_int() - 1,
                                                     card2.get_rank_int() - 1) +
               extra_flush;
    } else {
        return stat_table.mean_table.get_opp_crib(card1.get_rank_int() - 1,
                                                  card2.get_rank_int() - 1) +
               extra_flush;
    }
}

void GenerateCribStatistics::generate_probabilities() {
    generate_discard_probabilities();
}

void GenerateCribStatistics::calculate_stats() {
    calculate_discard_score_distribution();
    calculate_other_stats();
}

void GenerateCribStatistics::calculate_other_stats() {
    // Order matters
    calculate_max_min_tables();
    calculate_mean_tables();
    calculate_median_tables();
    calculate_variance_tables();
    calculate_std_dev_tables();
}

void GenerateCribStatistics::calculate_mean_tables() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                  t == 0 ? stat_table.score_dist_table.get_dealer_crib(i, j)
                         : stat_table.score_dist_table.get_opp_crib(i, j);
                float& mean = t == 0
                                ? stat_table.mean_table.get_dealer_crib(i, j)
                                : stat_table.mean_table.get_opp_crib(i, j);

                mean = 0;
                for (int k = 0; k < 30; k++) {
                    mean += k * current_score_dist_table_my_crib[k];
                }
            }
        }
    }
}

void GenerateCribStatistics::calculate_median_tables() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                  t == 0 ? stat_table.score_dist_table.get_dealer_crib(i, j)
                         : stat_table.score_dist_table.get_opp_crib(i, j);
                int& table_median =
                  t == 0 ? stat_table.median_table.get_dealer_crib(i, j)
                         : stat_table.median_table.get_opp_crib(i, j);

                float median = 0;
                float sum = 0;
                for (int k = 0; k < 30; k++) {
                    sum += current_score_dist_table_my_crib[k];
                    if (sum >= 0.5) {
                        median = k;
                        break;
                    }
                }
                table_median = median;
            }
        }
    }
}

void GenerateCribStatistics::calculate_variance_tables() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                  t == 0 ? stat_table.score_dist_table.get_dealer_crib(i, j)
                         : stat_table.score_dist_table.get_opp_crib(i, j);
                float& variance =
                  t == 0 ? stat_table.variance_table.get_dealer_crib(i, j)
                         : stat_table.variance_table.get_opp_crib(i, j);

                float mean = i == 0
                               ? stat_table.mean_table.get_dealer_crib(i, j)
                               : stat_table.mean_table.get_opp_crib(i, j);
                variance = 0;
                for (int k = 0; k < 30; k++) {
                    variance += (k - mean) * (k - mean) *
                                current_score_dist_table_my_crib[k];
                }
            }
        }
    }
}

void GenerateCribStatistics::calculate_std_dev_tables() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            ScoreDistributionTable current_score_dist_table_my_crib =
              stat_table.score_dist_table.get_dealer_crib(i, j);
            float variance = stat_table.variance_table.get_dealer_crib(i, j);
            stat_table.std_dev_table.get_dealer_crib(i, j) = sqrt(variance);

            ScoreDistributionTable current_score_dist_table_opp_crib =
              stat_table.score_dist_table.get_opp_crib(i, j);
            variance = stat_table.variance_table.get_opp_crib(i, j);
            stat_table.std_dev_table.get_opp_crib(i, j) = sqrt(variance);
        }
    }
}

void GenerateCribStatistics::calculate_max_min_tables() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                  t == 0 ? stat_table.score_dist_table.get_dealer_crib(i, j)
                         : stat_table.score_dist_table.get_opp_crib(i, j);
                int& max = t == 0 ? stat_table.max_table.get_dealer_crib(i, j)
                                  : stat_table.max_table.get_opp_crib(i, j);
                int& min = t == 0 ? stat_table.min_table.get_dealer_crib(i, j)
                                  : stat_table.min_table.get_opp_crib(i, j);

                max = 0;
                min = 0;
                for (int k = 0; k <= 29; k++) {
                    if (current_score_dist_table_my_crib[k] > 0) {
                        min = k;
                        break;
                    }
                }
                for (int k = 29; k >= 0; k--) {
                    if (current_score_dist_table_my_crib[k] > 0) {
                        max = k;
                        break;
                    }
                }
            }
        }
    }
}

void GenerateCribStatistics::generate_discard_probabilities() {
    std::cout << "Generating Tables..." << std::endl;
    Deck deck;
    deck.shuffle();

    CribDiscardProbabilities generating_prob_table = stat_table.prob_table;

    int total_games = 2 << 19;
    int epochs = 16;
    int num_games_in_epoch = total_games / epochs;
    for (int epoch = 0; epoch < epochs; epoch++) {

        generating_prob_table.to_freq_table();

        for (int i = 0; i < num_games_in_epoch; i++) {
            // Print Percentage
            if (num_games_in_epoch > 10 &&
                i % (num_games_in_epoch / 100) == 0) {
                std::cout << "\r"
                          << (int)(100 *
                                   (float)(epoch * num_games_in_epoch + i) /
                                   total_games)
                          << "%";
                std::cout.flush();
            }

            // TODO: Keep track of suits??

            deck.shuffleTopCardsIntoDeck();
            Hand dealer_hand = deck.deal_hand(6);
            dealer->set_hand(dealer_hand);
            auto [dealer_discard1, dealer_discard2] =
              dealer->make_discards(true, stat_table.prob_table);
            generating_prob_table.get_table_value(
              dealer_discard1.get_rank(), dealer_discard2.get_rank(), true)++;

            Hand pone_hand = deck.deal_hand(6);
            pone->set_hand(pone_hand);
            auto [pone_discard1, pone_discard2] =
              pone->make_discards(false, stat_table.prob_table);
            generating_prob_table.get_table_value(
              pone_discard1.get_rank(), pone_discard2.get_rank(), false)++;
        }

        generating_prob_table.num_sim_discards += num_games_in_epoch;

        generating_prob_table.to_prob_table();

        stat_table.prob_table = generating_prob_table;
    }
}

void GenerateCribStatistics::calculate_discard_score_distribution() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            stat_table.score_dist_table.get_dealer_crib(i, j).clear();
            stat_table.score_dist_table.get_opp_crib(i, j).clear();
        }
    }
    const Deck deck;  // For cut card
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                ScoreDistributionTable& current_score_freq_table =
                  t == 0 ? stat_table.score_dist_table.get_dealer_crib(i, j)
                         : stat_table.score_dist_table.get_opp_crib(i, j);

                for (int k = 0; k < 13; k++) {
                    for (int l = k; l < 13; l++) {
                        float freq =
                          t == 0
                            ? stat_table.prob_table.get_table_value(k, l, true)
                            : stat_table.prob_table.get_table_value(k, l,
                                                                    false);
                        for (auto it = deck.cbegin(); it != deck.cbegin() + 13;
                             ++it) {
                            Card cut = *it;
                            int cut_rank_int = cut.get_rank_int() - 1;
                            if (cut_rank_int == i && i == j && j == k &&
                                k == l) {
                                continue;
                            }

                            int cut_num = 4;
                            if (cut_rank_int == i)
                                cut_num--;
                            if (cut_rank_int == j)
                                cut_num--;
                            if (cut_rank_int == k)
                                cut_num--;
                            if (cut_rank_int == l)
                                cut_num--;
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
                            hand.add_card(Card(static_cast<Suit>(0),
                                               static_cast<Rank>(i + 1)));
                            hand.add_card(Card(static_cast<Suit>(0),
                                               static_cast<Rank>(j + 1)));
                            hand.add_card(Card(static_cast<Suit>(0),
                                               static_cast<Rank>(k + 1)));
                            hand.add_card(Card(static_cast<Suit>(0),
                                               static_cast<Rank>(l + 1)));
                            int score = score_hand_suitless(hand, cut);
                            current_score_freq_table[score] +=
                              (1 - knob_prob) * cut_prob * freq;
                            current_score_freq_table[score + 1] +=
                              (knob_prob)*cut_prob * freq;
                        }
                    }
                }
                current_score_freq_table.normalize();
            }
        }
    }
}

} // namespace cribbage
