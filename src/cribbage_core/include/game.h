#pragma once

#include <memory>

#include "board.h"
#include "hand.h"
#include "pgn/game_pgn.h"
#include "player.h"

namespace cribbage {

class Game {
  public:
    Game(Player* player1, Player* player2);
    //~Game() = default;

    bool play_game();

    const GamePgn& get_pgn() const { return pgn; }

  private:
    void play_round();
    void play_dealer_turn();
    void play_non_dealer_turn();
    void play_crib_turn();
    void play_cut_turn();
    void play_play_turn();
    void play_show_turn();

    void print_score() const;

  private:
    Player* player1;
    Player* player2;
    GamePgn pgn;

    Board board;
    int round_number;
};

}  // namespace cribbage
