#include "game.h"

namespace cribbage {

Game::Game(Player* player1, Player* player2) : player1(player1), player2(player2) {}

bool Game::play_game() {
    bool winner = false;

    while (true) {
        play_round();
        if (board.get_winner().has_value()) {
            winner = board.get_winner().value() == WhichPlayer::FIRST_DEALER;
            break;
        }
    }

    return winner;
}

void Game::play_round() {
    round_number++;

    play_dealer_turn();
    play_non_dealer_turn();
    play_crib_turn();
    play_cut_turn();
    play_play_turn();
    play_show_turn();
}

}  // namespace cribbage
