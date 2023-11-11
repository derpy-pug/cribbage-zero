#include "board.h"

#include <algorithm>
#include <iostream>

Board::Board()
    : player1({0, 0}), player2({0, 0})
{
}

Board::peg_pair& Board::get_pegs(Player player) 
{
    return player == Player::ONE ? player1 : player2;
}

const Board::peg_pair& Board::get_pegs(Player player) const
{
    return player == Player::ONE ? player1 : player2;
}

void Board::move(Player player, int holes)
{
    if (holes < 0) {
        std::cerr << "Cannot move pegs backwards" << std::endl;
        return;
    }
    if (get_winner().has_value()) {
        std::cerr << "Cannot move pegs after game is over" << std::endl;
        return;
    }

    peg_pair& pegs = get_pegs(player);
    int temp = pegs.front;
    pegs.front = pegs.front + holes;
    if (pegs.front > 121) {
        pegs.front = 121;
    }
    pegs.back = temp;
}

int Board::get_holes_from_end(Player player) const
{
    const peg_pair& pegs = get_pegs(player);
    return 121 - pegs.front;
}

int Board::get_holes_from_start(Player player) const
{
    const peg_pair& pegs = get_pegs(player);
    return pegs.front;
}

std::optional<Player> Board::get_winner() const
{
    if (player1.front == 121) {
        return Player::ONE;
    }
    if (player2.front == 121) {
        return Player::TWO;
    }
    return std::nullopt;
}
