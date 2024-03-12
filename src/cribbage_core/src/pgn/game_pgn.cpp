#include "pgn/game_pgn.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>
#include <string>
#include "scoring.h"
#include "util/parse.h"

#define PGN_DIR "pgn/"

static const int NUM_PLAYERS = 2;

static const std::array<std::string, 7> INFO_TAGS = {
  "event", "site", "date", "round", "firstdealer", "firstpone", "result"};

/*
 * List of possible round tags.
 *  H1: Hand of player 1/first dealer
 *  H2: Hand of player 2/first pone
 *  D1: Discards of player 1/first dealer
 *  D2: Discards of player 2/first pone
 *  C: Cut card
 *  P: Pegging cards
 *  S1: Score of player 1/first dealer's hand
 *  S2: Score of player 2/first pone's hand
 *  SC: Score of crib
 *  SP: Scores of pegging
 *  SH: Score of cut card (his heels if cut card is a jack)
 */
static const std::array<std::string, 11> ROUND_TAGS = {
  "H1", "H2", "D1", "D2", "C", "P", "S1", "S2", "SC", "SP", "SH"};

namespace cribbage {

void PGN::Round::add_pegging_card(Card card) {
    if (!pegging_cards) {
        pegging_cards = CardPile();
    }
    pegging_cards->add_card(card);
}

void PGN::Round::add_pegging_score(int score) {
    if (!pegging_scores) {
        pegging_scores = std::vector<int>();
    }
    pegging_scores->emplace_back(score);
}

PGN::ValidationType PGN::Round::validate() const {
    using RoundValidationType = PGN::Round::RoundValidationType;
    auto[hand1_validation, hand2_validation] = validate_hands();
    auto[discards1_validation, discards2_validation] = validate_discards();

    RoundValidationType hand1_score_validation;
    if (hand1_validation == VALID && discards1_validation == VALID) {
        Hand hand = hand1.value();
        hand.remove_card(discards1->first);
        hand.remove_card(discards1->second);
        hand1_score_validation = check_hands_score(hand, hand1_score.value());
    }
    RoundValidationType hand2_score_validation;
    if (hand2_validation == VALID && discards2_validation == VALID) {
        Hand hand = hand2.value();
        hand.remove_card(discards2->first);
        hand.remove_card(discards2->second);
        hand2_score_validation = check_hands_score(hand, hand2_score.value());
    }

    RoundValidationType crib_score_validation;
    if (discards1_validation == VALID && discards2_validation == VALID) {
        crib_score_validation = check_crib_score();
    }

    RoundValidationType cut_score_validation; 
    if (cut) {
        cut_score_validation = check_cut_score();
    }

    RoundValidationType pegging_cards_validation = check_pegging_cards();
    RoundValidationType pegging_scores_validation; 
    if (pegging_cards_validation == VALID) {
        pegging_scores_validation = check_pegging_score();
    }
    RoundValidationType pegging_player_validation;
    if (pegging_cards_validation == VALID) {
        pegging_player_validation = check_pegging_player();
    }


    // Now we know if each part is valid relative to itself, 
    // we can check if each part is valid relative to the other parts

    if (hand1_validation == VALID && discards1_validation == VALID) {
        bool contains = hand1->contains(discards1->first) && hand1->contains(discards1->second);
        if (!contains) {
            return ValidationType::FAIL;
        }
    }

    if (hand1_validation == RECONSTRUCT && discards1_validation == VALID) {
        if (hand1->size() == 5) {
            int num_contains = 0;
            for (const Card& card : hand1.value()) {
                if (card == discards1->first || card == discards1->second) {
                    num_contains++;
                }
            }
            if (num_contains != 1) {
                hand1_validation = INCOMPLETE;
                discards1_validation = INVALID;
            }
        } else if (hand1->size() == 4) {
            bool contains = hand1->contains(discards1->first) || hand1->contains(discards1->second);
            if (contains) {
                hand1_validation = INCOMPLETE;;
                discards1_validation = INVALID;
            } 
        }
    }

    if (hand2_validation == VALID && discards2_validation == VALID) {
        bool contains = hand2->contains(discards2->first) && hand2->contains(discards2->second);
        if (!contains) {
            return ValidationType::FAIL;
        }
    }

    if (hand2_validation == RECONSTRUCT && discards2_validation == VALID) {
        if (hand2->size() == 5) {
            int num_contains = 0;
            for (const Card& card : hand2.value()) {
                if (card == discards2->first || card == discards2->second) {
                    num_contains++;
                }
            } 
            if (num_contains != 1) {
                hand2_validation = INCOMPLETE;
                discards2_validation = INVALID;
            }
        } else if (hand2->size() == 4) {
            bool contains = hand2->contains(discards2->first) || hand2->contains(discards2->second);
            if (contains) {
                hand2_validation = INCOMPLETE;;
                discards2_validation = INVALID;
            } 
        }
    }


    return ValidationType::FINSIHED;
}

std::pair<PGN::Round::RoundValidationType, PGN::Round::RoundValidationType> PGN::Round::validate_hands() const{
    std::pair<PGN::Round::RoundValidationType, PGN::Round::RoundValidationType> validation = std::make_pair(RoundValidationType::INVALID, RoundValidationType::INVALID);
    if (!hand1) {
        validation.first = RoundValidationType::MISSING;
    } else {
        Hand hand = hand1.value();
        bool unqiue = std::unique(hand.begin(), hand.end()) == hand.end();
        if (!unqiue) {
            validation.first = RoundValidationType::INVALID;
        } else if (hand.size() == 6) {
            validation.first = RoundValidationType::VALID;
        } else if (hand.size() == 5) {
            validation.first = RoundValidationType::RECONSTRUCT;
        } else if (hand.size() == 4) {
            validation.first = RoundValidationType::RECONSTRUCT;
        } else {
            validation.first = RoundValidationType::INCOMPLETE;
        }

    }
    if (!hand2) {
        validation.second = RoundValidationType::MISSING;
    } else {
        Hand hand = hand2.value();
        bool unqiue = std::unique(hand.begin(), hand.end()) == hand.end();
        if (!unqiue) {
            validation.second = RoundValidationType::INVALID;
        } else if (hand.size() == 6) {
            validation.second = RoundValidationType::VALID;
        } else if (hand.size() == 5) {
            validation.second = RoundValidationType::RECONSTRUCT;
        } else if (hand.size() == 4) {
            validation.second = RoundValidationType::RECONSTRUCT;
        } else {
            validation.second = RoundValidationType::INCOMPLETE;
        }
    }
    return validation;
}
std::pair<PGN::Round::RoundValidationType, PGN::Round::RoundValidationType> PGN::Round::validate_discards() const {
    std::pair<PGN::Round::RoundValidationType, PGN::Round::RoundValidationType> validation = std::make_pair(RoundValidationType::INVALID, RoundValidationType::INVALID);

    if (!discards1) {
        validation.first = RoundValidationType::MISSING;
    } else {
        if (discards1->first == discards1->second) {
            validation.first = RoundValidationType::INVALID;
        } else {
            validation.first = RoundValidationType::VALID;
        }
    }

    if (!discards2) {
        validation.second = RoundValidationType::MISSING;
    } else {
        if (discards2->first == discards2->second) {
            validation.second = RoundValidationType::INVALID;
        } else {
            validation.second = RoundValidationType::VALID;
        }
    }

    return validation;
}
PGN::Round::RoundValidationType PGN::Round::check_crib_score() const {
    if (discards1->first == discards2->first ||
        discards1->first == discards2->second ||
        discards1->second == discards2->first ||
        discards1->second == discards2->second) {
        return RoundValidationType::INVALID;
    }

    Hand crib;
    crib.add_card(discards1->first);
    crib.add_card(discards1->second);
    crib.add_card(discards2->first);
    crib.add_card(discards2->second);
    crib.add_card(cut.value());
    int score = score_hand(crib, cut.value(), true);

    if (score == crib_score) {
        return RoundValidationType::VALID;
    } else {
        return RoundValidationType::SCORE_MISMATCH;
    }

}

PGN::Round::RoundValidationType PGN::Round::check_hands_score(Hand hand, int hand_score) const {

    int score = score_hand(hand, cut.value(), false);
    if (score == hand1_score) {
        return RoundValidationType::VALID;
    } else {
        return RoundValidationType::SCORE_MISMATCH;
    }
}
PGN::Round::RoundValidationType PGN::Round::check_cut_score() const {
    if (!cut_score) {
        return RoundValidationType::MISSING;
    } else {
        if (cut.value().get_rank() == Rank::JACK) {
            if (cut_score == 2) {
                return RoundValidationType::VALID;
            } else {
                return RoundValidationType::SCORE_MISMATCH;
            }
        } else {
            if (cut_score == 0) {
                return RoundValidationType::VALID;
            } else {
                return RoundValidationType::SCORE_MISMATCH;
            }
        }
    }
}
PGN::Round::RoundValidationType PGN::Round::check_pegging_score() const {
    if (!pegging_scores) {
        return RoundValidationType::MISSING;
    } else {
        if (pegging_cards.value().get_scores().size() != pegging_scores.value().size()) {
            return RoundValidationType::INCOMPLETE;
        } else {
            for (int i = 0; (unsigned)i < pegging_scores.value().size(); i++) {
                if (pegging_cards.value().get_scores()[i] != pegging_scores.value()[i]) {
                    return RoundValidationType::SCORE_MISMATCH;
                }
            }
            return RoundValidationType::VALID;
        }
    }
}
PGN::Round::RoundValidationType PGN::Round::check_pegging_cards() const {
    if (!pegging_cards) {
        return RoundValidationType::MISSING;
    } else {
        if (pegging_cards.value().get_cards().size() != NUM_PLAYERS * 4) {
            return RoundValidationType::INCOMPLETE;
        } else {
            auto cards = pegging_cards.value().get_cards();
            bool unique = std::unique(cards.begin(), cards.end()) == cards.end();
            if (!unique) {
                return RoundValidationType::INVALID;
            } else {
                return RoundValidationType::VALID;
            }
        }
    }
}
PGN::Round::RoundValidationType PGN::Round::check_pegging_player() const {
    if (!pegging_player) {
        return RoundValidationType::MISSING;
    } else {
        if (pegging_player.value().size() != pegging_cards.value().get_cards().size()) {
            return RoundValidationType::INCOMPLETE;
        } else {
            return RoundValidationType::VALID;
        }
    }
}

PGN::PGN(GameInfo game_info) : game_info(game_info) {}

/*
 * @brief Saves the game in PGN format.
 *
 * Append the game to the end of the file.
 */
bool PGN::save(std::string filename) const {
    std::ofstream pgn_file;
    pgn_file.open(PGN_DIR + filename, std::ios_base::app);
    if (!pgn_file.is_open()) {
        return false;
    }
    pgn_file << make_pgn();
    pgn_file.close();
    return true;
}

/*
 * @brief Checks if the line is a tag.
 *
 * @param line The line to check.
 *
 * @return 0 if not a line number or the line number if it is.
 */
static int get_round_number(const std::string& line) {
    // if tag is of the form "{number}.", then it is a round number
    if (line.size() > 1 && line.back() == '.') {
        for (unsigned int i = 0; i < line.size() - 1; i++) {
            if (!isdigit(line[i])) {
                return 0;
            }
        }
        int line_number = std::stoi(line.substr(0, line.size() - 1));
        if (line_number <= 0) {
            std::cerr << "Error parsing round number on line " << line_number
                      << std::endl;
            throw std::invalid_argument("Invalid round number");
        }
        return line_number;
    }
    return 0;
}

/*
 * @brief Loads the game from PGN format.
 *
 * @param pgn The PGN file to load the game from.
 *
 * The round number must be included and must be in the format "{number}.\n".
 * Although there could be any number of spaces between the number and the
 * newline.
 *
 * @return The game loaded from the PGN file.
 */
void PGN::load(std::stringstream& pgn) {
    std::string line;
    int line_number = 0;

    GameInfo new_game_info;
    // Read the game info
    while (std::getline(pgn, line)) {
        line_number++;
        std::stringstream ss(line);
        std::string tag;
        if (!(ss >> tag)) {
            continue;
        }
        // lower case the tag
        std::transform(tag.begin(), tag.end(), tag.begin(), ::tolower);

        std::string value;
        std::string temp;
        while (ss >> temp) {
            if (temp == "]") {
                break;
            }
            if (value.empty()) {
                value = temp;
            } else {
                value += " " + temp;
            }
        }
        if (value.size() > 1 && value.back() == ']') {
            value.pop_back();
        }
        if (value.size() >= 2) {
            if (value.front() == '"') {
                value.erase(0, 1);
            }
            if (value.back() == '"') {
                value.pop_back();
            }
        }

        if (tag == "[event") {
            new_game_info.event = value;
        } else if (tag == "[site") {
            new_game_info.site = value;
        } else if (tag == "[date") {
            new_game_info.date = value;
        } else if (tag == "[round") {
            new_game_info.round = value;
        } else if (tag == "[firstdealer") {
            new_game_info.first_dealer_name = value;
        } else if (tag == "[firstdealertype") {
            std::transform(value.begin(), value.end(), value.begin(),
                           ::tolower);
            if (value == "human") {
                new_game_info.first_dealer_type = PlayerType::HUMAN;
            } else if (value == "random") {
                new_game_info.first_dealer_type = PlayerType::RANDOM;
            } else if (value == "stat") {
                new_game_info.first_dealer_type = PlayerType::STAT;
            } else {
                std::cerr << "Error parsing first dealer type (" << value
                          << ") on line " << line_number << std::endl;
                new_game_info.first_dealer_type = PlayerType::NONE;
            }
        } else if (tag == "[firstpone") {
            new_game_info.first_pone_name = value;
        } else if (tag == "[firstponetype") {
            std::transform(value.begin(), value.end(), value.begin(),
                           ::tolower);
            if (value == "human") {
                new_game_info.first_pone_type = PlayerType::HUMAN;
            } else if (value == "random") {
                new_game_info.first_pone_type = PlayerType::RANDOM;
            } else if (value == "stat") {
                new_game_info.first_pone_type = PlayerType::STAT;
            } else {
                std::cerr << "Error parsing first pone type (" << value
                          << ") on line " << line_number << std::endl;
                new_game_info.first_pone_type = PlayerType::NONE;
            }
        } else if (tag == "[result") {
            std::string result;
            ss >> result;
            if (value == "1-0") {
                new_game_info.result = GameResult::FIRST_DEALER;
            } else if (value == "0-1") {
                new_game_info.result = GameResult::FIRST_PONE;
            } else {
                new_game_info.result = GameResult::NONE;
            }
        } else if (tag[0] == '[') {
            // Ignore other tags
            std::cerr << "Unkown info tag " << tag << " on line " << line_number
                      << std::endl;
            continue;
        } else {
            // Done reading game info
            break;
        }
    }

    this->game_info = new_game_info;

    Round round;
    int round_number = -1;
    bool is_round_number = false;
    bool use_line = true;
    // Read the rounds
    while (use_line || std::getline(pgn, line)) {
        if (!use_line) {
            line_number++;
        }
        use_line = false;
        std::stringstream ss(line);

        {
            std::stringstream ss_view(
              line);  // Just to see if the line is a round number
            std::string possible_round_number;
            ss_view >> possible_round_number;

            round_number = get_round_number(possible_round_number);
            is_round_number = round_number != 0;
            if (is_round_number) {
                if (round.round_number > 0) {
                    // If this is not the first round
                    add_round(round);
                    round = Round();
                }
                round.round_number = round_number;
            }
        }

        // Read the line
        std::string value;
        Hand hand;
        CardPile pegging_cards;
        std::vector<int> pegging_scores;
        while (ss >> value) {
            if (is_round_number) {
                is_round_number = false;
                continue;
            }
            // If the value is a tag, update the tag and update round values
            if (std::find(ROUND_TAGS.begin(), ROUND_TAGS.end(), value) !=
                ROUND_TAGS.end()) {
                switch (value[0]) {
                    case 'H': {
                        if (value == "H1") {
                            std::stringstream ss_view(line);
                            std::string temp;
                            while (ss_view >> temp) {
                                if (temp == "H1") {
                                    break;
                                }
                            }
                            temp.clear();
                            while (ss_view >> value) {
                                if (std::find(ROUND_TAGS.begin(),
                                              ROUND_TAGS.end(),
                                              value) != ROUND_TAGS.end()) {
                                    break;
                                }
                                ss >> value;
                                temp += value;  // No spacing necessary
                            }
                            try {
                                hand = parse_hand(temp);
                            } catch (std::invalid_argument& e) {
                                std::cerr << "Error parsing hand " << value
                                          << " on line " << line_number
                                          << std::endl;
                                throw e;
                            }
                            round.hand1 = hand;
                            hand = Hand();
                        } else if (value == "H2") {
                            std::stringstream ss_view(line);
                            std::string temp;
                            while (ss_view >> temp) {
                                if (temp == "H2") {
                                    break;
                                }
                            }
                            temp.clear();
                            while (ss_view >> value) {
                                if (std::find(ROUND_TAGS.begin(),
                                              ROUND_TAGS.end(),
                                              value) != ROUND_TAGS.end()) {
                                    break;
                                }
                                ss >> value;
                                temp += value;  // No spacing necessary
                            }
                            try {
                                hand = parse_hand(temp);
                            } catch (std::invalid_argument& e) {
                                std::cerr << "Error parsing hand " << value
                                          << " on line " << line_number
                                          << std::endl;
                                throw e;
                            }
                            round.hand2 = hand;
                            hand = Hand();
                        }
                        break;
                    }
                    case 'D': {
                        Card card1;
                        Card card2;
                        std::string card1_str;
                        std::string card2_str;
                        ss >> card1_str >> card2_str;
                        try {
                            card1 = Card(card1_str);
                        } catch (std::invalid_argument& e) {
                            std::cerr << "Error parsing card " << card1_str
                                      << " on line " << line_number
                                      << std::endl;
                            throw e;
                        }
                        try {
                            card2 = Card(card2_str);
                        } catch (std::invalid_argument& e) {
                            std::cerr << "Error parsing card " << card2_str
                                      << " on line " << line_number
                                      << std::endl;
                            throw e;
                        }

                        auto discards = std::make_pair(card1, card2);
                        auto& round_discards =
                          value == "D1" ? round.discards1 : round.discards2;
                        round_discards = discards;
                        break;
                    }
                    case 'C': {
                        if (value == "C") {
                            Card card;
                            std::string card_str;
                            ss >> card_str;
                            try {
                                card = Card(card_str);
                            } catch (std::invalid_argument& e) {
                                std::cerr << "Error parsing cut card "
                                          << card_str << " on line "
                                          << line_number << std::endl;
                                throw e;
                            }
                            round.cut = card;
                        }
                        break;
                    }
                    case 'P': {
                        std::string card_str;
                        while (ss >> card_str) {
                            if (card_str == "|") {
                                continue;
                            }
                            try {
                                Card card = Card(card_str);
                                pegging_cards.add_card(card);
                            } catch (std::invalid_argument& e) {
                                std::cerr << "Error parsing pegging card "
                                          << card_str << " on line "
                                          << line_number << std::endl;
                                throw e;
                            }
                        }
                        round.pegging_cards = pegging_cards;
                        break;
                    }
                    case 'S': {
                        if (value == "S1") {
                            int score;
                            ss >> score;
                            round.hand1_score = score;
                        } else if (value == "S2") {
                            int score;
                            ss >> score;
                            round.hand2_score = score;
                        } else if (value == "SC") {
                            int score;
                            ss >> score;
                            round.crib_score = score;
                        } else if (value == "SH") {
                            int score;
                            ss >> score;
                            round.cut_score = score;
                        } else if (value == "SP") {
                            int score;
                            while (ss >> score) {
                                pegging_scores.emplace_back(score);
                            }
                            round.pegging_scores = pegging_scores;
                        }
                        break;
                    }
                }
            } else {
                std::cerr << "Error parsing line " << line_number << std::endl;
                throw std::invalid_argument("Invalid line/tag");
            }
        }
    }
    add_round(round);
}

void PGN::add_round(Round&& round) {
    rounds.emplace_back(round);
    std::sort(rounds.begin(), rounds.end(), [](const Round& a, const Round& b) {
        return a.round_number < b.round_number;
    });
}

void PGN::add_round(const Round& round) {
    rounds.emplace_back(round);
    std::sort(rounds.begin(), rounds.end(), [](const Round& a, const Round& b) {
        return a.round_number < b.round_number;
    });
}

PGN::ValidationType set_min_validation(PGN::ValidationType& min_validation,
                                       PGN::ValidationType new_validation) {
    if (new_validation < min_validation) {
        min_validation = new_validation;
    }
    return min_validation;
}

PGN::ValidationType PGN::validate() const {
    // TODO: Validate game info

    ValidationType min_validation = ValidationType::FINSIHED;

    int i = 1;
    for (const Round& round : rounds) {
        bool last_round = (unsigned)i == rounds.size();

        if (round.round_number != i) {
            set_min_validation(min_validation, ValidationType::MISSING_ROUND);
        }

        if (!round.hand1 && !round.hand1_score) {
            set_min_validation(min_validation, ValidationType::MISSING_SCORES);
        }
        if (!round.hand2 && !round.hand2_score) {
            set_min_validation(min_validation, ValidationType::MISSING_SCORES);
        }
        if ((!round.discards1 || !round.discards2) && !round.crib_score) {
            set_min_validation(min_validation, ValidationType::MISSING_SCORES);
        }
        if (!round.cut && !round.cut_score) {  // Possibly could be skipped
            set_min_validation(min_validation, ValidationType::MISSING_SCORES);
        }
        if (!round.pegging_cards && !round.pegging_scores) {
            set_min_validation(min_validation, ValidationType::MISSING_SCORES);
        }

        if (round.hand1) {
            if (round.hand1_score) {
                if (round.hand1->size() != 6) {
                    set_min_validation(min_validation, ValidationType::PARTIAL);
                }
            } else {
                if (round.hand1->size() != 6) {
                    set_min_validation(min_validation, ValidationType::FAIL);
                }
            }

            if (round.discards1) {
                if (!round.hand1->contains(round.discards1->first) ||
                    !round.hand1->contains(round.discards1->second)) {
                    set_min_validation(min_validation, ValidationType::FAIL);
                }
            }
        }
        if (round.hand2) {
            if (round.hand2_score) {
                if (round.hand2->size() != 6) {
                    set_min_validation(min_validation, ValidationType::PARTIAL);
                }
            } else {
                if (round.hand2->size() != 6) {
                    set_min_validation(min_validation, ValidationType::FAIL);
                }
            }

            if (round.discards2) {
                if (round.hand2->size() == 6) {
                    if (!round.hand2->contains(round.discards2->first) ||
                        !round.hand2->contains(round.discards2->second)) {
                        set_min_validation(min_validation,
                                           ValidationType::FAIL);
                    }
                } else if (round.hand2->size() == 5) {
                    int num_matched = 0;
                    for (const Card& card : round.hand2.value().get_cards()) {
                        if (card == round.discards2->first ||
                            card == round.discards2->second) {
                            num_matched++;
                        }
                    }
                    if (num_matched == 1) {}

                } else {
                    set_min_validation(min_validation, ValidationType::PARTIAL);
                }
            }
        }
        if (round.discards1) {
            if (round.discards1->first == round.discards1->second) {
                set_min_validation(min_validation, ValidationType::FAIL);
            }
        }
        if (round.discards2) {
            if (round.discards2->first == round.discards2->second) {
                set_min_validation(min_validation, ValidationType::FAIL);
            }
        }

        // Make sure all cards are unique and all cards are used in the peggings
        Hand combined_cards;
        if (round.hand1) {
            for (const Card& card : round.hand1.value()) {
                combined_cards.add_card(card);
            }
        }
        if (round.hand2) {
            for (const Card& card : round.hand2.value()) {
                combined_cards.add_card(card);
            }
        }
        if (round.cut) {
            combined_cards.add_card(round.cut.value());
        }

        if (std::unique(combined_cards.begin(), combined_cards.end()) !=
            combined_cards.end()) {
            set_min_validation(min_validation, ValidationType::FAIL);
        }

        if (round.hand1 && round.hand2) {
            if (round.cut) {
                combined_cards.remove_card(round.cut.value());
            }

            // If the combined cards are not the same as the pegging cards
            if (round.pegging_cards) {
                if (combined_cards != round.pegging_cards->get_cards()) {
                    set_min_validation(min_validation, ValidationType::PARTIAL);
                }
            }

            bool removed_discards = false;
            if (round.discards1) {
                combined_cards.remove_card(round.discards1->first);
                combined_cards.remove_card(round.discards1->second);
                removed_discards = true;
            }
            if (round.discards2) {
                combined_cards.remove_card(round.discards2->first);
                combined_cards.remove_card(round.discards2->second);
                removed_discards = true;
            }

            if (removed_discards) {
                // If the combined cards are not the same as the pegging cards
                if (round.pegging_cards) {
                    if (combined_cards != round.pegging_cards->get_cards()) {}
                }
            }
        }

        auto cards = round.pegging_cards.value_or(CardPile()).get_cards();
        if (std::find(cards.begin(), cards.end(), round.cut.value()) !=
            cards.end()) {
            set_min_validation(min_validation, ValidationType::FAIL);
        }

        i++;
    }

    return ValidationType::FINSIHED;
}

static std::string pegging_cards_string(std::vector<Card> pegging_cards) {
    std::stringstream ss;
    for (const Card& card : pegging_cards) {
        ss << card << " ";
    }
    return ss.str();
}

static std::stringstream make_pgn_info_tags(const PGN::GameInfo& game_info) {
    std::stringstream ss;
    if (game_info.event != "") {
        ss << "[Event \"" + game_info.event + "\"]\n";
    }
    if (game_info.site != "") {
        ss << "[Site \"" + game_info.site + "\"]\n";
    }
    if (game_info.date != "") {
        ss << "[Date \"" + game_info.date + "\"]\n";
    }
    if (game_info.round != "") {
        ss << "[Round \"" + game_info.round + "\"]\n";
    }
    if (game_info.first_dealer_name != "") {
        ss << "[FirstDealer \"" + game_info.first_dealer_name + "\"]\n";
    }
    if (game_info.first_dealer_type != PlayerType::NONE) {
        ss << "[FirstDealerType \"";
        if (game_info.first_dealer_type == PlayerType::HUMAN) {
            ss << "Human";
        } else if (game_info.first_dealer_type == PlayerType::RANDOM) {
            ss << "Random";
        } else if (game_info.first_dealer_type == PlayerType::STAT) {
            ss << "Stat";
        }
        ss << "\"]\n";
    }
    if (game_info.first_pone_name != "") {
        ss << "[FirstPone \"" + game_info.first_pone_name + "\"]\n";
    }
    if (game_info.first_pone_type != PlayerType::NONE) {
        ss << "[FirstPoneType \"";
        if (game_info.first_pone_type == PlayerType::HUMAN) {
            ss << "Human";
        } else if (game_info.first_pone_type == PlayerType::RANDOM) {
            ss << "Random";
        } else if (game_info.first_pone_type == PlayerType::STAT) {
            ss << "Stat";
        }
        ss << "\"]\n";
    }
    if (game_info.result != PGN::GameResult::NONE) {
        ss << "[Result \"";
        if (game_info.result == PGN::GameResult::FIRST_DEALER) {
            ss << "1-0";
        } else if (game_info.result == PGN::GameResult::FIRST_PONE) {
            ss << "0-1";
        }
        ss << "\"]\n";
    }
    return ss;
}

static std::stringstream make_pgn_round_tags(const PGN::Round& round) {
    const std::string PGN_INDENT = "  ";  // Round tags indent
    const std::string PGN_SPACING =
      "  ";  // Spacing between tag and value groups
    std::stringstream ss;
    if (round.hand1) {
        ss << PGN_INDENT;
        ss << "H1 " << round.hand1->to_string();
        if (round.discards1) {
            ss << PGN_SPACING << "D1 " << round.discards1->first << " "
               << round.discards1->second;
        }
        if (round.hand1_score) {
            ss << PGN_SPACING << "S1 " << *round.hand1_score;
        }
    }
    if (round.hand2) {
        ss << "\n" << PGN_INDENT;
        ss << "H2 " << round.hand2->to_string();
        if (round.discards2) {
            ss << PGN_SPACING << "D2 " << round.discards2->first << " "
               << round.discards2->second;
        }
        if (round.hand2_score) {
            ss << PGN_SPACING << "S2 " << *round.hand2_score;
        }
    }
    if (round.crib_score) {
        ss << "\n" << PGN_INDENT;
        ss << "SC " << *round.crib_score;
    }
    if (round.cut) {
        ss << "\n" << PGN_INDENT;
        ss << "C " << *round.cut;
        if (round.cut_score) {
            ss << PGN_SPACING << "SH " << *round.cut_score;
        }
    }
    if (round.pegging_cards) {
        ss << "\n" << PGN_INDENT;
        ss << "P " << round.pegging_cards->to_string();
    }
    if (round.pegging_scores) {
        ss << "\n" << PGN_INDENT;
        ss << "SP ";
        for (int score : round.pegging_scores.value()) {
            ss << score << " ";
        }
    }
    return ss;
}

static std::stringstream make_pgn_rounds(
  const std::vector<PGN::Round>& rounds) {
    std::stringstream ss;
    unsigned int round_idx = 1;
    for (const PGN::Round& round : rounds) {
        ss << round_idx << ".\n";
        ss << make_pgn_round_tags(round).str();
        if (round_idx != rounds.size()) {
            ss << "\n\n";
        }

        round_idx++;
    }
    return ss;
}

/*
 * @brief Makes the PGN string for the cribbage game.
 *
 * Makes a standard format PGN string for the game.
 *
 * Example:
 * [Event "Example Event"]
 * [FirstDealer "Player1"]
 * [FirstPone "Player2"]
 * [Result "1-0"]
 *
 * 1. 
 *    H1 4H 3H 7D 7H 8C 8S D1 4H 3H
 *    H2 2D 3D 9H 3C TD QC D2 TD QD
 *    S3 6
 *    C 5S
 *    P 2D 7D 3C 8C 9H | 7H 3D 8S  
 *
 * 2.
 *    H 4H 3H 7D 7H 8C 8S S 12
 *    H 2D 3D 9H 3C TD QC S 6
 *    C 5S
 *    P 2D 7D 3C 8C 9H | 7H 3D 8S
 *    S 0  0  0  0  1    0  0  1
 *
 */
std::string PGN::make_pgn() const {
    // TODO: Colorize the PGN
    std::stringstream pgn;
    pgn << make_pgn_info_tags(game_info).str();

    pgn << "\n";

    pgn << make_pgn_rounds(rounds).str();

    pgn << "\n";

    return pgn.str();
}

std::ostream& operator<<(std::ostream& os, const PGN& game_pgn) {
    return os << game_pgn.make_pgn();
}

}  // namespace cribbage
