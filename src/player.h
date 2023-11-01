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

    /*
     * @brief Play a card in the pegging phase.
     */
    virtual Card play_card() = 0;

    /*
     * @brief Discard exactly 2 cards from the player's hand.
     */
    virtual Hand make_discards(bool is_my_crib) = 0;

private:
    std::string name;
    Hand hand;
};

class HumanPlayer : public Player
{
public:
    HumanPlayer(std::string name);

    Card play_card();
    Hand make_discards(bool is_my_crib);
};

class RandomPlayer : public Player
{
public:
    RandomPlayer(std::string name);

    Card play_card();
    Hand make_discards(bool is_my_crib);
};

class AIPlayer : public Player
{
public:
    AIPlayer(std::string name);

    Card play_card();
    Hand make_discards(bool is_my_crib);
};

#endif // PLAYER_H
