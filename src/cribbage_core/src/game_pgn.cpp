#include "game_pgn.h"

#include <sstream>


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

GamePgn::GamePgn(Player* player1, Player* player2, GameInfo game_info)
    : player1(player1), player2(player2), game_info(game_info) {}

bool GamePgn::save() {
    return false;
}

bool GamePgn::load() {
    return false;
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
 *    H 2D 3D 9H 3C TD QC D TD QD
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
    pgn << "[Event \"" + game_info.event + "\"]\n";
    pgn << "[Site \"" + game_info.site + "\"]\n";
    pgn << "[Date \"" + game_info.date + "\"]\n";
    pgn << "[Round \"" + game_info.round + "\"]\n";
    pgn << "[first_dealer \"" + game_info.first_dealer + "\"]\n";
    pgn << "[first_pone \"" + game_info.first_pone + "\"]\n";
    pgn << "[Result \"" + game_info.result + "\"]\n";
    pgn << "\n";

    int round_idx = 1;
    for (const Round& round : rounds) {
        pgn << round_idx << ".\n";

        if (round.hand1) {
            pgn << "  ";
            pgn << "H1 " << *round.hand1;
            if (round.discards1) {
                pgn << " D1 " << round.discards1->first << " " << round.discards1->second;
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
                pgn << " D2 " << round.discards2->first << " " << round.discards2->second;
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
