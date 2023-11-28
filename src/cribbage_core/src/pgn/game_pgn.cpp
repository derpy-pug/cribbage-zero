#include "pgn/game_pgn.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <sstream>
#include <string>

#define PGN_DIR "pgn/"

static const std::array<std::string, 7> INFO_TAGS = {
  "event", "site", "date", "round", "firstdealer", "firstpone", "result"};

static const std::array<std::string, 11> ROUND_TAGS = {
  "H1", "H2", "D1", "D2", "C", "P", "S1", "S2", "SC", "SP", "SH"};

namespace cribbage {

void GamePgn::Round::add_pegging_card(Card card) {
    if (!pegging_cards) {
        pegging_cards = CardPile();
    }
    pegging_cards->add_card(card);
}

void GamePgn::Round::add_pegging_score(int score) {
    if (!pegging_scores) {
        pegging_scores = std::vector<int>();
    }
    pegging_scores->emplace_back(score);
}

GamePgn::GamePgn(GameInfo game_info) : game_info(game_info) {}

/*
 * @brief Saves the game in PGN format.
 *
 * Append the game to the end of the file.
 */
bool GamePgn::save(std::string filename) {
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
GamePgn GamePgn::load(std::istream pgn) {
    std::string line;
    int line_number = 0;

    GameInfo game_info;
    // Read the game info
    while (std::getline(pgn, line)) {
        line_number++;
        std::stringstream ss(line);
        std::string tag;
        std::string value;
        ss >> tag;
        while (ss >> value) {
            tag += " " + value;
        }
        if (value.back() == ']' && value.size() > 1) {
            value.pop_back();
        }
        if (tag == "[Event") {
            game_info.event = value;
        } else if (tag == "[Site") {
            game_info.site = value;
        } else if (tag == "[Date") {
            game_info.date = value;
        } else if (tag == "[Round") {
            game_info.round = value;
        } else if (tag == "[FirstDealer") {
            game_info.first_dealer_name = value;
        } else if (tag == "[FirstPone") {
            game_info.first_pone_name = value;
        } else if (tag == "[Result") {
            std::string result;
            ss >> result;
            if (value == "1-0") {
                game_info.result = GameResult::WIN;
            } else if (value == "0-1") {
                game_info.result = GameResult::LOSS;
            } else {
                game_info.result = GameResult::NONE;
            }
        } else if (tag[0] == '[') {
            // Ignore other tags
            continue;
        } else if (tag == "") {
            // Ignore empty lines
            continue;
        } else {
            // Done reading game info
            break;
        }
    }

    GamePgn game_pgn(game_info);

    Round round;
    int round_number = -1;
    bool use_line = true;
    // Read the rounds
    while (use_line || std::getline(pgn, line)) {
        if (!use_line) {
            line_number++;
        }
        use_line = false;
        std::stringstream ss(line);
        std::string tag;
        ss >> tag;

        // if tag is of the form "{number}.", then it is a round number
        bool is_round_number = true;
        if (tag.size() > 1) {
            for (unsigned int i = 0; i < tag.size() - 1; i++) {
                if (!isdigit(tag[i])) {
                    is_round_number = false;
                    break;
                }
            }
            if (is_round_number && tag[tag.size() - 1] == '.') {
                int new_round_number = std::stoi(tag.substr(0, tag.size() - 1));
                if (round_number <= 0) {
                    std::cerr << "Error parsing round number on line "
                        << line_number << std::endl;
                    throw std::invalid_argument("Invalid round number");
                }
                round_number = new_round_number;

            } else {
                is_round_number = false;
            }
        } else {
            is_round_number = false;
        }
        if (is_round_number) {
            if (round.round_number != 0) { // If this is not the first round
                game_pgn.add_round(round);
                round = Round();
            }
            round.round_number = round_number;
        }

        // Read the round
        std::string value;
        Hand hand;
        CardPile pegging_cards;
        std::vector<int> pegging_scores;
        while (ss >> value) {
            // If the value is a tag, update the tag and update round values
            if (std::find(ROUND_TAGS.begin(), ROUND_TAGS.end(), value) !=
                ROUND_TAGS.end()) {
                switch (value[0]) {
                    case 'H': {
                        if (value == "H1") {
                            round.hand1 = hand;
                            hand = Hand();
                        } else if (value == "H2") {
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
                        } else if (value == "SP"){
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
                // Otherwise, add the value to the hand
                try {
                    Card card = Card(value);
                    hand.add_card(card);
                } catch (std::invalid_argument& e) {
                    std::cerr << "Error parsing card " << value << " on line "
                              << line_number << std::endl;
                    throw e;
                }
            }
        }
    }
    game_pgn.add_round(round);

    return game_pgn;
}

void GamePgn::add_round(Round&& round) {
    rounds.emplace_back(round);
}

void GamePgn::add_round(const Round& round) {
    rounds.emplace_back(round);
}

std::string pegging_cards_string(std::vector<Card> pegging_cards) {
    std::stringstream ss;
    for (const Card& card : pegging_cards) {
        ss << card << " ";
    }
    return ss.str();
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
std::string GamePgn::make_pgn() const {
    std::stringstream pgn;
    if (game_info.event != "") {
        pgn << "[Event \"" + game_info.event + "\"]\n";
    }
    if (game_info.site != "") {
        pgn << "[Site \"" + game_info.site + "\"]\n";
    }
    if (game_info.date != "") {
        pgn << "[Date \"" + game_info.date + "\"]\n";
    }
    if (game_info.round != "") {
        pgn << "[Round \"" + game_info.round + "\"]\n";
    }
    if (game_info.first_dealer_name != "") {
        pgn << "[FirstDealer \"" + game_info.first_dealer_name + "\"]\n";
    }
    if (game_info.first_pone_name != "") {
        pgn << "[FirstPone \"" + game_info.first_pone_name + "\"]\n";
    }
    if (game_info.result != GameResult::NONE) {
        pgn << "[Result \"";
        if (game_info.result == GameResult::WIN) {
            pgn << "1-0";
        } else if (game_info.result == GameResult::LOSS) {
            pgn << "0-1";
        }
        pgn << "\"]\n";
    }

    int round_idx = 1;
    for (const Round& round : rounds) {
        pgn << round_idx << ".\n";

        if (round.hand1) {
            pgn << "  ";
            pgn << "H1 " << *round.hand1;
            if (round.discards1) {
                pgn << " D1 " << round.discards1->first << " "
                    << round.discards1->second;
            }
            if (round.hand1_score) {
                pgn << " S1 " << *round.hand1_score;
            }
            pgn << "\n";
        }

        if (round.hand2) {
            pgn << "  ";
            pgn << "H2 " << *round.hand2;
            if (round.discards2) {
                pgn << " D2 " << round.discards2->first << " "
                    << round.discards2->second;
            }
            if (round.hand2_score) {
                pgn << " S2 " << *round.hand2_score;
            }
            pgn << "\n";
        }

        if (round.cut) {
            pgn << "  ";
            pgn << "C " << *round.cut;
            pgn << "\n";
        }

        if (round.pegging_cards) {
            pgn << "  ";
            pgn << "P ";
            pgn << round.pegging_cards->to_string();
            pgn << "\n";
        }

        if (round.pegging_scores) {
            pgn << "  ";
            pgn << "S ";
            for (int score : *round.pegging_scores) {
                pgn << score << " ";
            }
            pgn << "\n";
        }

        round_idx++;
    }

    return pgn.str();
}

std::ostream& operator<<(std::ostream& os, const GamePgn& game_pgn) {
    return os << game_pgn.make_pgn();
}

}  // namespace cribbage
