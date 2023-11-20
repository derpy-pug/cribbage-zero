#pragma once

#include <memory>

#include "hand.h"
#include "player.h"

namespace cribbage {

class Game {
  public:
    Game();
    Game(Player* player1, Player* player2);
    //~Game() = default;

    void play_game();

  private:
    void play_round();
    void play_dealer_turn();
    void play_non_dealer_turn();
    void play_crib_turn();
    void play_cut_turn();
    void play_play_turn();
    void play_show_turn();

    void deal_cards();
    void deal_cards(Hand& hand);
    void deal_cards(Hand& hand, int num_cards);

    void print_score() const;

  private:
    Player* player1;
    Player* player2;
};

} // namespace cribbage
