#pragma once

#include <memory>

#include "board.h"
#include "deck.h"
#include "generate_statistics.h"
#include "hand.h"
#include "pgn/game_pgn.h"
#include "player.h"

namespace cribbage {

class Game {
  public:
    Game(Player* player1, Player* player2,
         GenerateCribStatistics* gen_crib_stats);
    //~Game() = default;

    bool play_game();

    const GamePgn& get_pgn() const { return pgn; }

  private:
    void play_round();

    void deal();
    void discard();
    void score();

    bool is_player1_dealer() const { return round_number % 2 == 1; }

    void print_score() const;

  private:
    [[nodiscard]] bool pegging();
    [[nodiscard]] bool can_play_any_card(const Hand& hand,
                                         const CardPile& pegging_cards) const;
    [[nodiscard]] Card play_card(Player* player, const Hand& dealt_hand,
                                 Card cut, const CardPile& pegging_cards);
    [[nodiscard]] bool score_pegging(bool is_player1_turn,
                                     const CardPile& pegging_cards,
                                     bool is_go = false);

  private:
    Player* player1;
    Player* player2;
    GenerateCribStatistics* gen_crib_stats;

    GamePgn::Round round;
    GamePgn pgn;

    Deck deck;
    Board board;
    int round_number;
};

}  // namespace cribbage
