#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include "card_pile.h"
#include "hand.h"

namespace cribbage {

class GenerateCribStatistics;

class Player {
  public:
    enum class PlayerType { NONE = 0, HUMAN, RANDOM, STAT };

  public:
    Player(std::string name);
    virtual ~Player() = default;

    std::string get_name() const;

    virtual PlayerType get_type() const { return PlayerType::NONE; }

    Hand& get_hand();
    void set_hand(Hand hand);

    /*
     * @brief Play a card in the pegging phase.
     *
     * @important Removes the card from the player's hand.
     * @important does not check if the card is a valid play.
     *
     * @param pile The pile of cards played so far.
     * @param dealt_hand The player's hand before discarding.
     *
     * @return The card played.
     */
    virtual Card play_card(const CardPile& pile, const Hand& dealt_hand,
                           Card cut) = 0;

    /*
     * @brief Discard exactly 2 cards from the player's hand.
     *
     * @important Removes the cards from the player's hand.
     *
     * @param is_dealer true if the player is the dealer (is it their crib?)
     * @param gen_crib_stats The crib statistics generator
     *
     * @return The 2 cards to discard.
     */
    virtual std::pair<Card, Card> make_discards(
      bool is_dealer, GenerateCribStatistics* gen_crib_stats) = 0;

  protected:
    std::string name;
    Hand hand;
};

class HumanPlayer : public Player {
  public:
    HumanPlayer(std::string name);

    PlayerType get_type() const override { return PlayerType::HUMAN; }

    Card play_card(const CardPile& pile, const Hand& dealt_hand,
                   Card cut) override;
    std::pair<Card, Card> make_discards(
      bool is_dealer, GenerateCribStatistics* gen_crib_stats) override;
};

class RandomPlayer : public Player {
  public:
    RandomPlayer(std::string name);

    PlayerType get_type() const override { return PlayerType::RANDOM; }

    Card play_card(const CardPile& pile, const Hand& dealt_hand,
                   Card cut) override;
    std::pair<Card, Card> make_discards(
      bool is_dealer, GenerateCribStatistics* gen_crib_stats) override;
};

class StatPlayer : public Player {
  public:
    StatPlayer(std::string name);

    PlayerType get_type() const override { return PlayerType::STAT; }

    Card play_card(const CardPile& pile, const Hand& dealt_hand,
                   Card cut) override;
    std::pair<Card, Card> make_discards(
      bool is_dealer, GenerateCribStatistics* gen_crib_stats) override;
};

}  // namespace cribbage

#endif  // PLAYER_H
