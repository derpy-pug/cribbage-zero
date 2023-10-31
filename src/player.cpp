#include "player.h"

#include <iostream>
#include <random>
#include <algorithm>

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

Card HumanPlayer::discard()
{
    std::cout << "Your hand: " << get_hand() << std::endl;
    std::cout << "Discard a card: ";
    int i;
    std::cin >> i;
    return get_hand().begin()[i];
}

RandomPlayer::RandomPlayer(std::string name)
    : Player(name)
{
}

static std::random_device rd;
static std::mt19937 gen(rd());

Card RandomPlayer::play_card()
{
    std::uniform_int_distribution<> dis(0, get_hand().size() - 1);
    int i = dis(gen);
    return get_hand().begin()[i];
}

Card RandomPlayer::discard()
{
    std::uniform_int_distribution<> dis(0, get_hand().size() - 1);
    int i = dis(gen);
    return get_hand().begin()[i];
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

Card AIPlayer::discard()
{
    //TODO: Implement AI
    return get_hand().begin()[0];
}
