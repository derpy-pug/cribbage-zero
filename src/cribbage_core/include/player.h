#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "hand.h"

namespace cribbage {

class GenerateCribStatistics;

class Player {
  public:
    Player(std::string name);
    virtual ~Player() = default;

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
    virtual std::pair<Card, Card> make_discards(
        bool is_dealer, GenerateCribStatistics* gen_crib_stats) = 0;

  private:
    std::string name;
    Hand hand;
};

class HumanPlayer : public Player {
  public:
    HumanPlayer(std::string name);

    Card play_card();
    std::pair<Card, Card> make_discards(bool is_dealer,
                                        GenerateCribStatistics* gen_crib_stats);
};

class RandomPlayer : public Player {
  public:
    RandomPlayer(std::string name);

    Card play_card();
    std::pair<Card, Card> make_discards(bool is_dealer,
                                        GenerateCribStatistics* gen_crib_stats);
};

class StatPlayer : public Player {
  public:
    StatPlayer(std::string name);

    Card play_card();
    std::pair<Card, Card> make_discards(bool is_dealer,
                                        GenerateCribStatistics* gen_crib_stats);
};

}  // namespace cribbage

#endif  // PLAYER_H
