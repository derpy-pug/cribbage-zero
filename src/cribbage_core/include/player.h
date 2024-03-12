#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include "card_pile.h"
#include "hand.h"

namespace cribbage {

class GenerateCribStatistics;

enum class PlayerType { NONE = 0, HUMAN, RANDOM, STAT };


class PlayerInfo {
  public:
    PlayerInfo() = default;
    PlayerInfo(std::string name, PlayerType type);

    const std::string& get_name() const { return name; }
    PlayerType get_type() const { return type; }

  private:
    std::string name;
    PlayerType type;
};

class CribDiscardProbabilities;

class Player {
  public:

  public:
    Player(PlayerInfo info);
    virtual ~Player() = default;

    const PlayerInfo& get_info() const { return info; }

    Hand& get_hand() { return hand; }
    void set_hand(Hand hand) { this->hand = hand; }

    /*
     * @brief Play a card in the pegging phase.
     *
     * @important Removes the card from the player's hand.
     * @important does check if the card is a valid play.
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
      bool is_dealer, const CribDiscardProbabilities& crib_discard_probs) = 0;

  protected:
    PlayerInfo info;
    Hand hand;
};

class PlayerFactory {
  public:
    static std::unique_ptr<Player> create_player(PlayerInfo info);
};

class HumanPlayer : public Player {
  public:
    HumanPlayer(PlayerInfo info) : Player(info) {}
    Card play_card(const CardPile& pile, const Hand& dealt_hand,
                   Card cut) override;
    std::pair<Card, Card> make_discards(
      bool is_dealer, const CribDiscardProbabilities& crib_discard_probs) override;
};

class RandomPlayer : public Player {
  public:
    RandomPlayer(PlayerInfo info) : Player(info) {}
    Card play_card(const CardPile& pile, const Hand& dealt_hand,
                   Card cut) override;
    std::pair<Card, Card> make_discards(
      bool is_dealer, const CribDiscardProbabilities& crib_discard_probs) override;
};

class StatPlayer : public Player {
  public:
    StatPlayer(PlayerInfo info) : Player(info) {}
    Card play_card(const CardPile& pile, const Hand& dealt_hand,
                   Card cut) override;
    std::pair<Card, Card> make_discards(
      bool is_dealer, const CribDiscardProbabilities& crib_discard_probs) override;
};

}  // namespace cribbage

#endif  // PLAYER_H
