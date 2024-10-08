#include "util/parse.h"
#include <algorithm>
#include <sstream>

namespace cribbage {

Card parse_card(std::string card_str) {
    if (card_str.size() != 2) {
        throw std::invalid_argument("Card string must be 2 characters");
    }

    // Upper case the string
    char rank_char = std::toupper(card_str[0]);
    char suit_char = std::toupper(card_str[1]);

    Rank rank;
    Suit suit;

    switch (rank_char) {
        case 'A':
        case '1':
            rank = Rank::ACE;
            break;
        case '2':
            rank = Rank::TWO;
            break;
        case '3':
            rank = Rank::THREE;
            break;
        case '4':
            rank = Rank::FOUR;
            break;
        case '5':
            rank = Rank::FIVE;
            break;
        case '6':
            rank = Rank::SIX;
            break;
        case '7':
            rank = Rank::SEVEN;
            break;
        case '8':
            rank = Rank::EIGHT;
            break;
        case '9':
            rank = Rank::NINE;
            break;
        case 'T':
            rank = Rank::TEN;
            break;
        case 'J':
            rank = Rank::JACK;
            break;
        case 'Q':
            rank = Rank::QUEEN;
            break;
        case 'K':
            rank = Rank::KING;
            break;
        case 'N':
            rank = Rank::NONE;
            break;
        default:
            throw std::invalid_argument("Invalid rank character");
    }

    switch (suit_char) {
        case 'C':
            suit = Suit::CLUBS;
            break;
        case 'D':
            suit = Suit::DIAMONDS;
            break;
        case 'H':
            suit = Suit::HEARTS;
            break;
        case 'S':
            suit = Suit::SPADES;
            break;
        case 'N':
            suit = Suit::NONE;
            break;
        default:
            throw std::invalid_argument("Invalid suit character");
    }


    return Card(suit, rank);
}

/*
 * @breif Parse a string into a Hand and a is_dealer bool
 *
 * The string should be a list of cards
 * Ex.
 *  "AS 2S 3S 4S 5S 6S" 
 *  "as 2s 3s 4s"
 *  "AS2S3S 4S 5s"
 *
 * @return A hand
 */
Hand parse_hand(std::string hand_str) {
    std::transform(hand_str.begin(), hand_str.end(), hand_str.begin(),
                   ::toupper);
    // Remove possible spaces
    hand_str.erase(std::remove(hand_str.begin(), hand_str.end(), ' '),
                   hand_str.end());
    if (hand_str.size() % 2 == 1) {
        throw std::invalid_argument("Hand input must be even length");
    }
    std::string card_str;
    Hand hand;
    // every two characters is a card
    size_t num_cards = hand_str.size() / 2;
    for (size_t i = 0; i < num_cards; i++) {
        card_str = hand_str.substr(i * 2, 2);
        hand.add_card(Card(card_str));
    }
    return hand;
}

bool parse_is_dealer(std::string is_dealer_str) {
    std::transform(is_dealer_str.begin(), is_dealer_str.end(),
            is_dealer_str.begin(), ::tolower);
    if (is_dealer_str == "1" || is_dealer_str == "true" ||
            is_dealer_str == "t" || is_dealer_str == "yes" ||
            is_dealer_str == "y") {
        return true;
    } else if (is_dealer_str == "0" || is_dealer_str == "false" ||
            is_dealer_str == "f" || is_dealer_str == "no" ||
            is_dealer_str == "n") {
        return false;
    } else {
        throw std::invalid_argument("Invalid is_dealer string");
    }
}

    std::pair<Card, Card> parse_discards(std::string discard_str) {
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

    std::pair<ScoreType, Statistic> parse_sort_by(std::string sort_by_str) {
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

        return {score_type, stat};
    }

}  // namespace cribbage
