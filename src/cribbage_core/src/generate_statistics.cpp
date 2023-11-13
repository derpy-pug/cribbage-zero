#include "generate_statistics.h"
#include "deck.h"

#include <math.h>
#include <algorithm>

#include "scoring.h"

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
    if (!mean_tables_generated) {
        generate_mean_tables();
    }
    if (is_dealer) {
        return mean_table.get_dealer_crib(card1.get_rank_int() - 1,
                                          card2.get_rank_int() - 1) +
               extra_flush;
    } else {
        return mean_table.get_opp_crib(card1.get_rank_int() - 1,
                                       card2.get_rank_int() - 1) +
               extra_flush;
    }
}

void GenerateCribStatistics::generate_all_tables() {
    generate_freq_tables();
    generate_all_but_freq_tables();
}

void GenerateCribStatistics::generate_all_but_freq_tables() {
    generate_max_min_tables();
    generate_mean_tables();
    generate_median_tables();
    generate_variance_tables();
    generate_std_dev_tables();
}

void GenerateCribStatistics::generate_mean_tables() {
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
    freq_tables_generated = true;
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                    t == 0 ? score_dist_table.get_dealer_crib(i, j)
                           : score_dist_table.get_opp_crib(i, j);
                float& mean = t == 0 ? mean_table.get_dealer_crib(i, j)
                                     : mean_table.get_opp_crib(i, j);

                mean = 0;
                for (int k = 0; k < 30; k++) {
                    mean += k * current_score_dist_table_my_crib[k];
                }
            }
        }
    }
}

void GenerateCribStatistics::generate_median_tables() {
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                    t == 0 ? score_dist_table.get_dealer_crib(i, j)
                           : score_dist_table.get_opp_crib(i, j);
                int& table_median = t == 0 ? median_table.get_dealer_crib(i, j)
                                           : median_table.get_opp_crib(i, j);

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

void GenerateCribStatistics::generate_variance_tables() {
    if (!mean_tables_generated) {
        generate_mean_tables();
    }
    variance_tables_generated = true;
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                    t == 0 ? score_dist_table.get_dealer_crib(i, j)
                           : score_dist_table.get_opp_crib(i, j);
                float& variance = t == 0 ? variance_table.get_dealer_crib(i, j)
                                         : variance_table.get_opp_crib(i, j);

                float mean = i == 0 ? mean_table.get_dealer_crib(i, j)
                                    : mean_table.get_opp_crib(i, j);
                variance = 0;
                for (int k = 0; k < 30; k++) {
                    variance += (k - mean) * (k - mean) *
                                current_score_dist_table_my_crib[k];
                }
            }
        }
    }
}

void GenerateCribStatistics::generate_std_dev_tables() {
    if (!variance_tables_generated) {
        generate_variance_tables();
    }
    variance_tables_generated = true;
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            ScoreDistributionTable current_score_dist_table_my_crib =
                score_dist_table.get_dealer_crib(i, j);
            float variance = variance_table.get_dealer_crib(i, j);
            std_dev_table.get_dealer_crib(i, j) = sqrt(variance);

            ScoreDistributionTable current_score_dist_table_opp_crib =
                score_dist_table.get_opp_crib(i, j);
            variance = variance_table.get_opp_crib(i, j);
            std_dev_table.get_opp_crib(i, j) = sqrt(variance);
        }
    }
}

void GenerateCribStatistics::generate_max_min_tables() {
    if (!freq_tables_generated) {
        generate_freq_tables();
    }
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                const ScoreDistributionTable& current_score_dist_table_my_crib =
                    t == 0 ? score_dist_table.get_dealer_crib(i, j)
                           : score_dist_table.get_opp_crib(i, j);
                int& max = t == 0 ? max_table.get_dealer_crib(i, j)
                                  : max_table.get_opp_crib(i, j);
                int& min = t == 0 ? min_table.get_dealer_crib(i, j)
                                  : min_table.get_opp_crib(i, j);

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

void GenerateCribStatistics::generate_freq_tables() {
    generate_unscored_freq_tables();
    is_freq_table_loaded = false;
    generate_scored_freq_tables();
    freq_tables_generated = true;
}

void GenerateCribStatistics::generate_unscored_freq_tables() {
    std::cout << "Generating Tables..." << std::endl;
    Deck deck;
    deck.shuffle();
    int total_games = 2 << 19;
    int epochs = 16;
    int num_games_in_epoch = total_games / epochs;
    for (int epoch = 0; epoch < epochs; epoch++) {
        for (int i = 0; i < 13; i++) {
            for (int j = i; j < 13; j++) {
                freq_table.get_dealer_crib(i, j) *= freq_num_games;
                freq_table.get_opp_crib(i, j) *= freq_num_games;
            }
        }
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
                dealer->make_discards(true, this);
            int freq_i = dealer_discard1.get_rank_int() - 1;
            int freq_j = dealer_discard2.get_rank_int() - 1;
            if (freq_i > freq_j) {
                std::swap(freq_i, freq_j);
            }
            freq_table.get_dealer_crib(freq_i, freq_j)++;

            Hand pone_hand = deck.deal_hand(6);
            pone->set_hand(pone_hand);
            auto [pone_discard1, pone_discard2] =
                pone->make_discards(false, this);
            freq_i = pone_discard1.get_rank_int() - 1;
            freq_j = pone_discard2.get_rank_int() - 1;
            if (freq_i > freq_j) {
                std::swap(freq_i, freq_j);
            }
            freq_table.get_opp_crib(freq_i, freq_j)++;
        }

        freq_num_games += num_games_in_epoch;

        for (int i = 0; i < 13; i++) {
            for (int j = i; j < 13; j++) {
                freq_table.get_dealer_crib(i, j) /= freq_num_games;
                freq_table.get_opp_crib(i, j) /= freq_num_games;
            }
        }
    }
}

void GenerateCribStatistics::generate_scored_freq_tables() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            score_dist_table.get_dealer_crib(i, j).clear();
            score_dist_table.get_opp_crib(i, j).clear();
        }
    }
    const Deck deck;  // For cut card
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            for (int t = 0; t < 2; t++) {
                ScoreDistributionTable& current_score_freq_table =
                    t == 0 ? score_dist_table.get_dealer_crib(i, j)
                           : score_dist_table.get_opp_crib(i, j);

                for (int k = 0; k < 13; k++) {
                    for (int l = k; l < 13; l++) {
                        float freq = t == 0 ? freq_table.get_dealer_crib(k, l)
                                            : freq_table.get_opp_crib(k, l);
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
            }
        }
    }
}

DiscardStatistics::DiscardStatistics(Player* player, Card discard1,
                                     Card discard2, bool is_dealer,
                                     GenerateCribStatistics* gen_crib_stats)
    : discard1(discard1),
      discard2(discard2),
      player(player),
      is_dealer(is_dealer),
      gen_crib_stats(gen_crib_stats),
      score_dist_hand(),
      score_dist_crib(),
      score_dist_combined(-60, 60) {}

void DiscardStatistics::generate_all_tables(std::optional<Card> optional_cut) {
    Deck deck = Deck();
    deck.remove_cards(player->get_hand());
    Hand crib;
    crib.add_card(discard1);
    crib.add_card(discard2);
    crib.add_card(Card(Suit::SPADES, Rank::ACE));  // Doesn't matter
    crib.add_card(Card(Suit::SPADES, Rank::ACE));  // Doesn't matter

    Hand hand = player->get_hand();
    hand.remove_card(discard1);
    hand.remove_card(discard2);

    int num_skipped = 0;
    // TODO: Try to make this more efficient by iterating only over A-K of Spades
    for (auto it = deck.cbegin(); it != deck.cend(); ++it) {
        // Score for hand
        Card cut = *it;
        if (optional_cut) {
            cut = *optional_cut;
        }

        int hand_score = score_hand(hand, cut, false);
        if (optional_cut) {
            score_dist_hand[hand_score] += 1.0;
        } else {
            score_dist_hand[hand_score] += 1.0 / 46.0;
        }

        // Score for crib and combined stats
        int cards_count[14] = {-1, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
        for (int i = 0; i < 6; i++) {
            cards_count[hand[i].get_rank_int()]--;
        }
        cards_count[cut.get_rank_int()]--;

        bool possible_flush = discard1.get_suit() == discard2.get_suit() &&
                              discard1.get_suit() == cut.get_suit();
        bool has_knob = discard1 == Card(cut.get_suit(), Rank::JACK) ||
                        discard2 == Card(cut.get_suit(), Rank::JACK);
        for (int i = 1; i < 14; i++) {
            if (cards_count[i] == 0) {
                num_skipped++;
                continue;
            }
            crib[2] = Card(Suit::SPADES, static_cast<Rank>(i));
            cards_count[i]--;
            for (int j = i; j < 14; j++) {
                if (cards_count[j] == 0) {
                    num_skipped++;
                    continue;
                }
                crib[3] = Card(Suit::HEARTS, static_cast<Rank>(j));
                cards_count[j]--;

                // j and k need to be different ranks and different to the cut rank
                // j and k cards need to not be in the hand
                bool is_flush = 0;
                if (possible_flush) {
                    if (i != cut.get_rank_int() && j != cut.get_rank_int() &&
                        i != j) {
                        is_flush = 1;
                        if (hand.contains(
                                Card(cut.get_suit(), static_cast<Rank>(i)))) {
                            is_flush = 0;
                        }
                        if (hand.contains(
                                Card(cut.get_suit(), static_cast<Rank>(j)))) {
                            is_flush = 0;
                        }
                    }
                }
                float prob_flush = 0;
                if (is_flush) {
                    prob_flush = 1.0 / (cards_count[i] + 1);
                    prob_flush *= 1.0 / (cards_count[j] + 1);
                }

                float prob_knob = has_knob;
                bool possible_knob = has_knob;
                if (!possible_knob && (i == 11 || j == 11) &&
                    cut.get_rank() != Rank::JACK) {
                    possible_knob = 1;
                    if (hand.contains(Card(cut.get_suit(), Rank::JACK))) {
                        possible_knob = 0;
                    }
                    int num_jacks = (i == 11) + (j == 11);  // 1 or 2
                    prob_knob =
                        (float)num_jacks / (cards_count[11] + num_jacks);
                }

                float prob =
                    gen_crib_stats->get_freq(crib[2], crib[3], !is_dealer);
                if (!optional_cut) {
                    prob *= 1.0 / 46;
                }
                int crib_score = score_hand_suitless(crib, cut);
                int combined_score = is_dealer ? hand_score + crib_score
                                               : hand_score - crib_score;
                score_dist_crib[crib_score] +=
                    prob * (1 - prob_flush) * (1 - prob_knob);
                score_dist_combined[combined_score] +=
                    prob * (1 - prob_flush) * (1 - prob_knob);

                crib_score += possible_knob;
                combined_score = is_dealer ? hand_score + crib_score
                                           : hand_score - crib_score;
                score_dist_crib[crib_score] +=
                    prob * (1 - prob_flush) * prob_knob;
                score_dist_combined[combined_score] +=
                    prob * (1 - prob_flush) * prob_knob;

                crib_score += is_flush ? 5 : 0;
                combined_score = is_dealer ? hand_score + crib_score
                                           : hand_score - crib_score;
                score_dist_crib[crib_score] += prob * prob_flush * prob_knob;
                score_dist_combined[combined_score] +=
                    prob * prob_flush * prob_knob;

                crib_score -= possible_knob;
                combined_score = is_dealer ? hand_score + crib_score
                                           : hand_score - crib_score;
                score_dist_crib[crib_score] +=
                    prob * prob_flush * (1 - prob_knob);
                score_dist_combined[combined_score] +=
                    prob * prob_flush * (1 - prob_knob);

                cards_count[j]++;
            }
            cards_count[i]++;
        }
        cards_count[cut.get_rank_int() - 1]++;
        if (optional_cut) {
            break;
        }
    }

    float sum = 0;
    for (int i = score_dist_combined.get_possible_score_min();
         i <= score_dist_combined.get_possible_score_max(); i++) {
        sum += score_dist_combined[i];
    }
    /* Normalize the probabilities
     * This is nessesary because some hands are skipped.
     * This normalization is not perfect and assumes
     * that the skipped hands prob is distributed evenly.
     */

    sum = 1 / sum;
    for (int i = score_dist_combined.get_possible_score_min();
         i <= score_dist_combined.get_possible_score_max(); i++) {
        score_dist_combined[i] *= sum;
    }

    /* std::cout << "Discards: " << discard1 << " " << discard2 << std::endl; */
    /* if (sum < 0.999 || sum > 1.001) { */
    /*     std::cerr << "Error: Probabilities do not sum to 1. Sum: " << sum << std::endl; */
    /* } */
    /* else { */
    /*     std::cout << "Probabilities sum to 1. Sum: " << sum << std::endl; */
    /* } */
    /* std::cout << num_skipped << " hands skipped" << std::endl; */
    /* std::cout << std::endl; */
}

void DiscardStatistics::generate_all_tables_simulated(Player* opponent,
                                                      std::optional<Card> cut) {
    bool is_opponent_nullptr = opponent == nullptr;
    if (is_opponent_nullptr) {
        opponent = new StatPlayer("Opponent");
    }
    Deck deck = Deck();
    deck.remove_cards(player->get_hand());
    if (cut) {
        deck.remove_card(*cut);
    }
    deck.shuffle();

    Hand hand = player->get_hand();
    hand.remove_card(discard1);
    hand.remove_card(discard2);

    Hand crib;
    crib.add_card(discard1);
    crib.add_card(discard2);
    crib.add_card(Card(Suit::SPADES, Rank::ACE));  // Doesn't matter
    crib.add_card(Card(Suit::SPADES, Rank::ACE));  // Doesn't matter
                                                   //

    // --- ONLY RESPONSIBLE FOR ENDING SIMULATION ---
    const int NUM_MEANS = 20;
    float means[NUM_MEANS];
    for (int i = 0; i < NUM_MEANS; i++) {  // Set variance to be high
        means[i] = i;
    }
    int current_mean_idx = 0;
    float mean_of_means = 10;

    float variance = 10;
    // --- END ONLY RESPONSIBLE FOR ENDING SIMULATION ---

    int i = 0;
    const int MAX_ITERATIONS = 3000;
    while (variance > 0.27 && i <= MAX_ITERATIONS) {
        for (int round = 0; round < 50; round++) {
            deck.shuffleTopCardsIntoDeck();
            // Score for hand
            Card cut_card = Card(Suit::SPADES, Rank::ACE);
            if (cut) {
                cut_card = *cut;
            } else {
                cut_card = deck.deal_card();
            }

            int hand_score = score_hand(hand, cut_card, false);
            score_dist_hand[hand_score] += 1.0;

            // Score for crib
            Hand crib_temp = deck.deal_hand(6);
            opponent->set_hand(crib_temp);
            auto [d1, d2] = opponent->make_discards(false, gen_crib_stats);
            crib[2] = d1;
            crib[3] = d2;
            int crib_score = score_hand(crib, cut_card, true);
            score_dist_crib[crib_score] += 1.0;

            // Score for combined stats
            int combined_score =
                is_dealer ? hand_score + crib_score : hand_score - crib_score;
            score_dist_combined[combined_score] += 1.0;

            i++;
        }
        std::cout << "\r" << i << " iterations";
        std::cout << " Variance: " << variance;
        std::cout.flush();

        // --- ONLY RESPONSIBLE FOR ENDING SIMULATION ---
        for (int j = score_dist_combined.get_possible_score_min();
             j <= score_dist_combined.get_possible_score_max(); j++) {
            score_dist_combined[j] /= i;
        }
        mean_of_means *= NUM_MEANS;
        mean_of_means -= means[current_mean_idx];
        means[current_mean_idx] = score_dist_combined.calc_mean();
        mean_of_means += means[current_mean_idx];
        mean_of_means /= NUM_MEANS;
        for (int j = score_dist_combined.get_possible_score_min();
             j <= score_dist_combined.get_possible_score_max(); j++) {
            score_dist_combined[j] *= i;
        }

        // Calc variance
        variance = 0;
        for (int i = 0; i < NUM_MEANS; i++) {
            variance += (means[i] - mean_of_means) * (means[i] - mean_of_means);
        }
        variance /= NUM_MEANS - 1;

        current_mean_idx = (current_mean_idx + 1) % NUM_MEANS;

        // --- END ONLY RESPONSIBLE FOR ENDING SIMULATION ---
    }

    // Make probabilities (They were counts before)
    for (int j = score_dist_combined.get_possible_score_min();
         j <= score_dist_combined.get_possible_score_max(); j++) {
        score_dist_combined[j] /= i;
    }
    for (int j = score_dist_hand.get_possible_score_min();
         j <= score_dist_hand.get_possible_score_max(); j++) {
        score_dist_hand[j] /= i;
    }
    for (int j = score_dist_crib.get_possible_score_min();
         j <= score_dist_crib.get_possible_score_max(); j++) {
        score_dist_crib[j] /= i;
    }

    if (is_opponent_nullptr) {
        delete opponent;
    }
}

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

GenerateDiscardStatistics::GenerateDiscardStatistics(
    Player* player, bool is_dealer, GenerateCribStatistics* gen_crib_stats)
    : discard_stats(),
      player(player),
      is_dealer(is_dealer),
      gen_crib_stats(gen_crib_stats) {}

void GenerateDiscardStatistics::generate_discard_stats(std::optional<Card> cut,
                                                       bool use_simulated,
                                                       Player* opponent) {
    for (auto it = player->get_hand().cbegin(); it != player->get_hand().cend();
         ++it) {
        for (auto it2 = it + 1; it2 != player->get_hand().cend(); ++it2) {
            Card discard1 = *it;
            Card discard2 = *it2;
            std::unique_ptr<DiscardStatistics> discard_stat =
                std::make_unique<DiscardStatistics>(player, discard1, discard2,
                                                    is_dealer, gen_crib_stats);
            if (use_simulated) {
                discard_stat->generate_all_tables_simulated(opponent, cut);
            } else {
                discard_stat->generate_all_tables(cut);
            }
            discard_stats.emplace_back(std::move(discard_stat));
        }
    }
}

const DiscardStatistics& GenerateDiscardStatistics::get_discard_stats(
    Card discard1, Card discard2) const {
    for (auto it = discard_stats.cbegin(); it != discard_stats.cend(); ++it) {
        if ((*it)->get_discard1() == discard1 &&
            (*it)->get_discard2() == discard2) {
            return **it;
        }
    }
    std::cerr << "Error: Discard statistics not found" << std::endl;
    throw std::exception();
}

void GenerateDiscardStatistics::sort_discard_stats(ScoreType score_type,
                                                   Statistic stat) {
    switch (stat) {
        case Statistic::MEAN:
            std::sort(
                discard_stats.begin(), discard_stats.end(),
                [score_type](const std::unique_ptr<DiscardStatistics>& a,
                             const std::unique_ptr<DiscardStatistics>& b) {
                    return a->get_mean(score_type) > b->get_mean(score_type);
                });
            break;
        case Statistic::MEDIAN:
            std::sort(
                discard_stats.begin(), discard_stats.end(),
                [score_type](const std::unique_ptr<DiscardStatistics>& a,
                             const std::unique_ptr<DiscardStatistics>& b) {
                    return a->get_median(score_type) >
                           b->get_median(score_type);
                });
            break;
        case Statistic::VARIANCE:
            std::sort(
                discard_stats.begin(), discard_stats.end(),
                [score_type](const std::unique_ptr<DiscardStatistics>& a,
                             const std::unique_ptr<DiscardStatistics>& b) {
                    return a->get_variance(score_type) >
                           b->get_variance(score_type);
                });
            break;
        case Statistic::STD_DEV:
            std::sort(
                discard_stats.begin(), discard_stats.end(),
                [score_type](const std::unique_ptr<DiscardStatistics>& a,
                             const std::unique_ptr<DiscardStatistics>& b) {
                    return a->get_std_dev(score_type) >
                           b->get_std_dev(score_type);
                });
            break;
        case Statistic::MAX:
            std::sort(
                discard_stats.begin(), discard_stats.end(),
                [score_type](const std::unique_ptr<DiscardStatistics>& a,
                             const std::unique_ptr<DiscardStatistics>& b) {
                    return a->get_max(score_type) > b->get_max(score_type);
                });
            break;
        case Statistic::MIN:
            std::sort(
                discard_stats.begin(), discard_stats.end(),
                [score_type](const std::unique_ptr<DiscardStatistics>& a,
                             const std::unique_ptr<DiscardStatistics>& b) {
                    return a->get_min(score_type) > b->get_min(score_type);
                });
            break;
        default:
            std::cerr << "Error: Invalid statistic" << std::endl;
            exit(1);
    }
}

const DiscardStatistics& GenerateDiscardStatistics::get_best_discard_stats()
    const {
    return *discard_stats[0];
}

std::string GenerateDiscardStatistics::get_discard_stats_string(
    int num_discard_stats) const {
    std::string discard_stats_string = "";
    for (int i = 0; i < num_discard_stats; i++) {
        discard_stats_string += discard_stats[i]->get_discard_string();
        discard_stats_string += "\n";
    }
    return discard_stats_string;
}

void GenerateDiscardStatistics::print_discard_stats(
    int num_discard_stats) const {
    std::cout << get_discard_stats_string(num_discard_stats);
}

std::ostream& operator<<(std::ostream& os,
                         const GenerateDiscardStatistics& gen_discard_stats) {
    for (auto it = gen_discard_stats.discard_stats.cbegin();
         it != gen_discard_stats.discard_stats.cend(); ++it) {
        os << **it << std::endl;
    }
    return os;
}
