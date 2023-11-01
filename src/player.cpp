#include "player.h"

#include <iostream>
#include <algorithm>

#include "util/cribbage_random.h"

Player::Player(std::string name)
    : name(name)
{
}

std::string Player::get_name() const
{
    return name;
}

Hand& Player::get_hand()
{
    return hand;
}

void Player::set_hand(Hand hand)
{
    this->hand = hand;
}

HumanPlayer::HumanPlayer(std::string name)
    : Player(name)
{
}

Card HumanPlayer::play_card()
{
    std::cout << "Your hand: " << get_hand() << std::endl;
    std::cout << "Play a card: ";
    int i;
    std::cin >> i;
    return get_hand().begin()[i];
}

Hand HumanPlayer::make_discards(bool is_my_crib)
{
    if (is_my_crib) {
        std::cout << "Your crib" << std::endl;
    } else {
        std::cout << "Opponent's crib" << std::endl;
    }
    std::cout << "Your hand: " << get_hand() << std::endl;
    std::cout << "Discard two cards: ";
    int i, j;
    std::cin >> i;
    std::cin >> j;
    Hand discards;
    discards.add_card(get_hand()[i]);
    discards.add_card(get_hand()[j]);
    return discards;
}

RandomPlayer::RandomPlayer(std::string name)
    : Player(name)
{
}

Card RandomPlayer::play_card()
{
    int i = CribbageRandom::get_instance()->get_random_int(0, get_hand().size());
    return get_hand().begin()[i];
}

Hand RandomPlayer::make_discards(bool is_my_crib)
{
    Hand discards;
    int i = CribbageRandom::get_instance()->get_random_int(0, get_hand().size());
    int j = CribbageRandom::get_instance()->get_random_int(0, get_hand().size() - 1);
    if (j == i) j = get_hand().size() - 1;
    discards.add_card(get_hand()[i]);
    discards.add_card(get_hand()[j]);
    return discards;
}

AIPlayer::AIPlayer(std::string name)
    : Player(name)
{
}

Card AIPlayer::play_card()
{
    //TODO: Implement AI
    return get_hand().begin()[0];
}

Hand AIPlayer::make_discards(bool is_my_crib)
{
    //TODO: Implement AI
    Hand discards;
    discards.add_card(get_hand()[0]);
    discards.add_card(get_hand()[1]);
    return discards;
}
