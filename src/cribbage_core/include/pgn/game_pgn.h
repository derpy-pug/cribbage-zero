#pragma once

#include <istream>
#include <optional>
#include "card_pile.h"
#include "hand.h"
#include "player.h"

namespace cribbage {

/* 
 * @brief Class for saving and loading games in PGN format.
 */
class GamePgn {
  public:
    enum class GameResult { NONE = 0, FIRST_DEALER, FIRST_PONE };

    struct GameInfo {
        std::string event;
        std::string site;
        std::string date;
        std::string round;
        std::string first_dealer_name;
        Player::PlayerType first_dealer_type;
        std::string first_pone_name;
        Player::PlayerType first_pone_type;
        GameResult result;
    };

  public:
    /*
     * @brief Class for saving and loading rounds in PGN format.
     *
     * @important if any score is not provided, it will be automatically calculated. 
     *              Only use a score if you want to override the calculated score.
     */
    class Round {
      public:
        Round() = default;
        Round(const Round& other) = default;
        Round(Round&& other) = default;
        Round& operator=(const Round& other) = default;
        Round& operator=(Round&& other) = default;

        void add_pegging_card(Card card);
        void add_pegging_score(int score);

      public:
        int round_number;

        std::optional<Hand> hand1;
        std::optional<Hand> hand2;
        std::optional<int> hand1_score;
        std::optional<int> hand2_score;

        std::optional<std::pair<Card, Card>> discards1;
        std::optional<std::pair<Card, Card>> discards2;
        std::optional<int> crib_score;

        std::optional<Card> cut;
        std::optional<int> cut_score;  // For the his heels(cut jack).

        std::optional<CardPile> pegging_cards;
        std::optional<std::vector<int>> pegging_scores;
        std::vector<bool> pegging_player;
    };

  public:
    GamePgn() = default;
    GamePgn(GameInfo game_info);
    //~GamePgn() = default;

    bool save(std::string filename) const;
    static GamePgn load(std::istream pgn);

    void add_round(Round&& round);
    void add_round(const Round& round);

    void set_game_info(GameInfo game_info) { this->game_info = game_info; }

    std::string make_pgn() const noexcept(false);
    friend std::ostream& operator<<(std::ostream& os, const GamePgn& game_pgn);

  private:
    Player* player1;
    Player* player2;
    GameInfo game_info;

    std::vector<Round> rounds;
};

}  // namespace cribbage
