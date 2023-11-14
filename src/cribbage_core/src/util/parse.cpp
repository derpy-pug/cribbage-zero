#include "util/parse.h"
#include <algorithm>
#include <sstream>

namespace cribbage {

/*
 * @breif Parse a string into a Hand and a is_dealer bool
 *
 * @param hand_str A string of cards possibly separated by spaces
 *              and ending with a Y or N.
 *
 * @return A pair of a Hand and a bool.
 */
Hand string_to_hand(std::string hand_str) {
    std::transform(hand_str.begin(), hand_str.end(), hand_str.begin(),
                   ::toupper);
    // Remove possible spaces
    hand_str.erase(std::remove(hand_str.begin(), hand_str.end(), ' '),
                   hand_str.end());
    if (hand_str.size() != 12) {
        throw std::invalid_argument("Hand string must be 12 characters");
    }
    std::string card_str;
    Hand hand;
    // every two characters is a card
    for (int i = 0; i < 6; i++) {
        card_str = hand_str.substr(i * 2, 2);
        hand.add_card(Card(card_str));
    }
    return hand;
}

std::pair<Card, Card> string_to_discards(std::string discard_str) {
    std::transform(discard_str.begin(), discard_str.end(), discard_str.begin(),
                   ::toupper);
    // Remove possible spaces
    discard_str.erase(std::remove(discard_str.begin(), discard_str.end(), ' '),
                      discard_str.end());
    if (discard_str.size() != 4) {
        throw std::invalid_argument("Discard string must be 4 characters");
    }
    std::string card_str;
    Card card1 = Card(discard_str.substr(0, 2));
    Card card2 = Card(discard_str.substr(2, 2));
    return {card1, card2};
}

std::pair<Statistic, ScoreType> string_to_sort_by(std::string sort_by_str) {
    std::transform(sort_by_str.begin(), sort_by_str.end(), sort_by_str.begin(),
                   ::tolower);
    std::stringstream ss(sort_by_str);
    std::string stat_str;
    std::string score_type_str;
    ss >> stat_str;
    ss >> score_type_str;
    Statistic stat;
    ScoreType score_type;
    if (stat_str == "max") {
        stat = Statistic::MAX;
    } else if (stat_str == "min") {
        stat = Statistic::MIN;
    } else if (stat_str == "mean") {
        stat = Statistic::MEAN;
    } else if (stat_str == "median") {
        stat = Statistic::MEDIAN;
    } else if (stat_str == "variance") {
        stat = Statistic::VARIANCE;
    } else if (stat_str == "std_dev") {
        stat = Statistic::STD_DEV;
    } else {
        throw std::invalid_argument("Invalid statistic");
    }

    if (score_type_str == "hand") {
        score_type = ScoreType::HAND;
    } else if (score_type_str == "crib") {
        score_type = ScoreType::CRIB;
    } else if (score_type_str == "combined") {
        score_type = ScoreType::COMBINED;
    } else {
        throw std::invalid_argument("Invalid score type");
    }

    return {stat, score_type};
}

}  // namespace cribbage