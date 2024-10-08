#pragma once

#include <optional>
#include "card_pile.h"
#include "hand.h"
#include "player.h"

namespace cribbage {

/* 
 * @brief Class for saving and loading games in PGN format.
 */
class PGN {
  public:
    enum class GameResult { NONE = 0, FIRST_DEALER, FIRST_PONE };

    enum class ValidationType {
        INVALID,
        MISSING_ROUND,
        MISSING_SCORES,
        PARTIAL,
        UNFINISHED,
        FINISHED
    };

    struct GameInfo {
        GameInfo() = default;

        std::string event;
        std::string site;
        std::string date;
        std::string round;
        std::string first_dealer_name;
        PlayerType first_dealer_type;
        std::string first_pone_name;
        PlayerType first_pone_type;
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

        ValidationType validate() const;

        std::string to_string() const;
        friend std::ostream& operator<<(std::ostream& os, const Round& round);
      private:
        // Bitflags for hand validation
        
        enum RoundValidationType{
            INVALID = 0,
            VALID,
            SCORE_MISMATCH,
            INCOMPLETE,
            MISSING,
            RECONSTRUCT, 
            SINGLE,
        };

        std::pair<RoundValidationType, RoundValidationType> validate_hands() const;
        std::pair<RoundValidationType, RoundValidationType> validate_discards() const;
        RoundValidationType check_crib_score() const;
        RoundValidationType check_hands_score(Hand hand, int score) const;
        RoundValidationType check_cut_score() const;
        RoundValidationType check_pegging_score() const;
        RoundValidationType check_pegging_cards() const;
        RoundValidationType check_pegging_player() const;


      public:
        int round_number = 0;

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
        std::optional<std::vector<bool>> pegging_player;
    };

  public:
    PGN() = default;
    PGN(GameInfo game_info);
    //~GamePgn() = default;

    bool save(std::string filename) const;
    bool load(std::stringstream& pgn);

    void add_round(Round&& round);
    void add_round(const Round& round);

    void set_game_info(GameInfo game_info) { this->game_info = game_info; }

    ValidationType validate() const;

    const std::vector<Round>& get_rounds() const { return rounds; }

    std::string make_pgn() const noexcept(false);
    friend std::ostream& operator<<(std::ostream& os, const PGN& game_pgn);

  private:
    Player* player1;
    Player* player2;
    GameInfo game_info;

    std::vector<Round> rounds;
};

}  // namespace cribbage
