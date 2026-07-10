#include "statistics/generator/Discard.h"

#include "deck.h"
#include "scoring.h"
#include "statistics/generator/DiscardHeuristics.h"
#include "statistics/stats/CribDiscardProbabilities.h"

namespace cribbage {

GenerateDiscardStatistics::GenerateDiscardStatistics(
  Player* player, bool is_dealer,
  const CribDiscardProbabilities& crib_discard_probs)
    : all_discard_stats(),
      player(player),
      is_dealer(is_dealer),
      crib_discard_probs(crib_discard_probs) {}

Hand make_kept_hand(
    const Hand& original_hand,
    const Card& discard1,
    const Card& discard2
) {
    Hand kept_hand = original_hand;
    kept_hand.remove_card(discard1);
    kept_hand.remove_card(discard2);
    return kept_hand;
}

struct WeightedCardPair {
    Card first;
    Card second;
    double weight;
};

std::vector<WeightedCardPair> make_opponent_pairs(
    const Deck& unknown_cards,
    const CribDiscardProbabilities& probabilities,
    bool opponent_is_dealer
) {
    std::vector<WeightedCardPair> pairs;
    pairs.reserve(1035);

    for (auto first = unknown_cards.cbegin();
         first != unknown_cards.cend();
         ++first) {
        for (auto second = first + 1;
             second != unknown_cards.cend();
             ++second) {
            pairs.push_back({
                *first,
                *second,
                probabilities.get_prob(
                    *first,
                    *second,
                    opponent_is_dealer
                )
            });
        }
    }

    return pairs;
}

DiscardStatistics GenerateDiscardStatistics::generate_discard_stats(
    Card discard1,
    Card discard2,
    std::optional<Card> optional_cut
) {
    const Hand& original_hand = player->get_hand();

    DiscardStatistics result(
        discard1,
        discard2,
        is_dealer,
        player->get_info()
    );

    auto& hand_distribution = result.score_dist_hand;
    auto& crib_distribution = result.score_dist_crib;
    auto& combined_distribution = result.score_dist_combined;

    const Hand kept_hand =
        make_kept_hand(original_hand, discard1, discard2);

    Deck unknown_cards;
    unknown_cards.remove_cards(original_hand);

    // Put make_opponent_pairs here.
    // It runs once for this discard choice, not once per cut.
    const auto opponent_pairs =
        make_opponent_pairs(
            unknown_cards,
            crib_discard_probs,
            !is_dealer
        );

    auto process_cut =
        [&](const Card& cut, double cut_probability) {
            const int hand_score =
                score_hand(kept_hand, cut, false);

            hand_distribution[hand_score] +=
                cut_probability;

            double feasible_probability = 0.0;

            // First pass: calculate the total probability of pairs
            // that remain possible after choosing this cut.
            for (const WeightedCardPair& opponent : opponent_pairs) {
                if (opponent.first == cut ||
                    opponent.second == cut) {
                    continue;
                }

                feasible_probability += opponent.weight;
            }

            if (feasible_probability <= 0.0) {
                throw std::runtime_error(
                    "No feasible opponent discard outcomes"
                );
            }

            // Second pass: score each feasible physical discard pair.
            for (const WeightedCardPair& opponent : opponent_pairs) {
                if (opponent.first == cut ||
                    opponent.second == cut) {
                    continue;
                }

                const double opponent_probability =
                    opponent.weight /
                    feasible_probability;

                // Put score_crib here.
                // This replaces make_crib(...) followed by score_hand(...).
                const int crib_score =
                    score_crib(
                        discard1,
                        discard2,
                        opponent.first,
                        opponent.second,
                        cut
                    );

                const int combined_score =
                    is_dealer
                        ? hand_score + crib_score
                        : hand_score - crib_score;

                const double outcome_probability =
                    cut_probability *
                    opponent_probability;

                crib_distribution[crib_score] +=
                    outcome_probability;

                combined_distribution[combined_score] +=
                    outcome_probability;
            }
        };

    if (optional_cut.has_value()) {
        const Card cut = *optional_cut;

        if (original_hand.contains(cut)) {
            throw std::invalid_argument(
                "Cut card is already in the player's hand"
            );
        }

        process_cut(cut, 1.0);
    } else {
        const double cut_probability =
            1.0 /
            static_cast<double>(unknown_cards.size());

        for (const Card& cut : unknown_cards) {
            process_cut(cut, cut_probability);
        }
    }
    hand_distribution.update_score_bounds();
    crib_distribution.update_score_bounds();
    combined_distribution.update_score_bounds();
    return result;
}

DiscardStatistics GenerateDiscardStatistics::generate_discard_stats_legacy(
  Card discard1, Card discard2, std::optional<Card> optional_cut) {
    DiscardStatistics discard_stat(discard1, discard2, is_dealer,
                                   player->get_info());

    // Aliases
    ScoreDistributionTable& score_dist_hand = discard_stat.score_dist_hand;
    ScoreDistributionTable& score_dist_crib = discard_stat.score_dist_crib;
    ScoreDistributionTable& score_dist_combined =
      discard_stat.score_dist_combined;

    Deck deck = Deck();
    deck.remove_cards(player->get_hand());
    Hand crib;
    crib.add_card(discard1);
    crib.add_card(discard2);
    crib.add_card(Card(Suit::SPADES, Rank::ACE));  // Doesn't matter
    crib.add_card(Card(Suit::SPADES, Rank::ACE));  // Doesn't matter

    Hand hand = player->get_hand();
    Hand hand_full = hand;
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
            cards_count[hand_full[i].get_rank_int()]--;
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
            crib[2] = Card(Suit::SPADES, static_cast<Rank>(i)); // Suit doesn't matter
            cards_count[i]--;
            for (int j = i; j < 14; j++) {
                if (cards_count[j] == 0) {
                    num_skipped++;
                    continue;
                }
                crib[3] = Card(Suit::HEARTS, static_cast<Rank>(j)); // Suit doesn't matter
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
                  crib_discard_probs.get_prob(crib[2], crib[3], !is_dealer);
                if (!optional_cut) {
                    prob *= 1.0 / 46;
                }
                int crib_score = score_hand_suitless(crib, cut);
                int combined_score =
                  is_dealer ? hand_score + crib_score : hand_score - crib_score;
                score_dist_crib[crib_score] +=
                  prob * (1 - prob_flush) * (1 - prob_knob);
                score_dist_combined[combined_score] +=
                  prob * (1 - prob_flush) * (1 - prob_knob);

                crib_score += possible_knob;
                combined_score =
                  is_dealer ? hand_score + crib_score : hand_score - crib_score;
                score_dist_crib[crib_score] +=
                  prob * (1 - prob_flush) * prob_knob;
                score_dist_combined[combined_score] +=
                  prob * (1 - prob_flush) * prob_knob;

                crib_score += is_flush ? 5 : 0;
                combined_score =
                  is_dealer ? hand_score + crib_score : hand_score - crib_score;
                score_dist_crib[crib_score] += prob * prob_flush * prob_knob;
                score_dist_combined[combined_score] +=
                  prob * prob_flush * prob_knob;

                crib_score -= possible_knob;
                combined_score =
                  is_dealer ? hand_score + crib_score : hand_score - crib_score;
                score_dist_crib[crib_score] +=
                  prob * prob_flush * (1 - prob_knob);
                score_dist_combined[combined_score] +=
                  prob * prob_flush * (1 - prob_knob);

                cards_count[j]++;
            }
            cards_count[i]++;
        }
        if (optional_cut) {
            break;
        }
    }

    /* Normalize the probabilities
     * This is nessesary because some hands are skipped.
     * This normalization is not perfect and assumes
     * that the skipped hands prob is distributed evenly.
     *
     * Also the normalization sets the min and max scores for the dist
     */
    score_dist_combined.normalize();
    score_dist_hand.normalize();
    score_dist_crib.normalize();

    // auto calc_sum = [](const ScoreDistributionTable& dist) {
    //     float sum = 0.0f;

    //     for (int score = dist.get_possible_score_min();
    //     score <= dist.get_possible_score_max();
    //     ++score) {
    //         sum += dist[score];
    //     }

    //     return sum;
    // };

    // std::cout << "Hand:     " << calc_sum(score_dist_hand) << '\n';
    // std::cout << "Crib:     " << calc_sum(score_dist_crib) << '\n';
    // std::cout << "Combined: " << calc_sum(score_dist_combined) << '\n';
    // std::cout << "Skipped: " << num_skipped << '\n';
    // std::cout << std::endl;

    return discard_stat;
}

DiscardStatistics GenerateDiscardStatistics::generate_discard_stats_simulated(
  Card discard1, Card discard2, std::optional<Card> cut, Player* opponent) {
    DiscardStatistics discard_stat(discard1, discard2, is_dealer,
                                   player->get_info());

    // Aliases
    ScoreDistributionTable& score_dist_hand = discard_stat.score_dist_hand;
    ScoreDistributionTable& score_dist_crib = discard_stat.score_dist_crib;
    ScoreDistributionTable& score_dist_combined =
      discard_stat.score_dist_combined;

    bool is_opponent_nullptr = opponent == nullptr;
    if (is_opponent_nullptr) {
        opponent =
          PlayerFactory::create_player({"Stat", PlayerType::STAT}).release();
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
            auto [d1, d2] = opponent->make_discards(false, crib_discard_probs);
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
    score_dist_combined.normalize();
    score_dist_hand.normalize();
    score_dist_crib.normalize();

    if (is_opponent_nullptr) {
        delete opponent;
    }

    return discard_stat;
}
void GenerateDiscardStatistics::generate_all_discard_stats(
  std::optional<Card> cut, bool use_simulated, Player* opponent) {
    for (auto it = player->get_hand().cbegin(); it != player->get_hand().cend();
         ++it) {
        for (auto it2 = it + 1; it2 != player->get_hand().cend(); ++it2) {
            Card discard1 = *it;
            Card discard2 = *it2;
            if (use_simulated) {
                all_discard_stats.discard_stats.emplace_back(
                  generate_discard_stats_simulated(discard1, discard2, cut,
                                                   opponent));
            } else {
                all_discard_stats.discard_stats.emplace_back(
                  generate_discard_stats(discard1, discard2, cut));
            }
        }
    }
}
void GenerateDiscardStatistics::generate_heuristics(ScoreType score_type,
                             Statistic stat) {
    GenerateDiscardHeuristics gen_discard_heuristics(all_discard_stats);
    DiscardHeuristics heuristics = gen_discard_heuristics.generate_discard_heuristics(score_type, stat);

    for (auto it = all_discard_stats.discard_stats.begin();
         it != all_discard_stats.discard_stats.end(); ++it) {
        (*it).heuristic = heuristics.get_heuristic(*it);
                                                  
    }
}

} // namespace cribbage
