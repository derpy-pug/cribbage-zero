#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "hand.h"

class Player
{
public:
    Player(std::string name);

    std::string get_name() const;

    Hand& get_hand();
    void set_hand(Hand hand);

    virtual Card play_card() = 0;
    virtual Card discard() = 0;

private:
    std::string name;
    Hand hand;
};

class HumanPlayer : public Player
{
public:
    HumanPlayer(std::string name);

    Card play_card();
    Card discard();
};

class RandomPlayer : public Player
{
public:
    RandomPlayer(std::string name);

    Card play_card();
    Card discard();
};

class AIPlayer : public Player
{
public:
    AIPlayer(std::string name);

    Card play_card();
    Card discard();
};






#endif // PLAYER_H
