#include "player.h"

#include <algorithm>
#include <iostream>

#include "cribbage_random.h"
#include "generate_statistics.h"

namespace cribbage {

Player::Player(std::string name) : name(name) {}

std::string Player::get_name() const {
    return name;
}

Hand& Player::get_hand() {
    return hand;
}

void Player::set_hand(Hand hand) {
    this->hand = hand;
}

HumanPlayer::HumanPlayer(std::string name) : Player(name) {}

Card HumanPlayer::play_card(const CardPile& pile, const Hand& dealt_hand, Card cut) {
    std::cout << "Your hand: " << get_hand() << std::endl;
    std::cout << "Cut card: " << cut << std::endl;
    std::cout << "Play a card: ";
    unsigned int i;
    std::cin >> i;
    if (i >= get_hand().size()) {
        std::cout << "Invalid card index" << std::endl;
        return play_card(pile, dealt_hand, cut);
    }
    if (!pile.can_play_card(get_hand()[i])) {
        std::cout << "Sum over 31, can't play that card" << std::endl;
        return play_card(pile, dealt_hand, cut);
    }
    return get_hand()[i];
}

std::pair<Card, Card> HumanPlayer::make_discards(
  bool is_dealer, GenerateCribStatistics* gen_crib_stats) {
    if (is_dealer) {
        std::cout << "Your crib" << std::endl;
    } else {
        std::cout << "Opponent's crib" << std::endl;
    }
    std::cout << "Your hand: " << get_hand() << std::endl;
    std::cout << "Discard two cards: ";
    unsigned int i, j;
    std::cin >> i;
    std::cin >> j;
    if (i == j || i >= get_hand().size() ||
        j >= get_hand().size()) {
        std::cout << "Invalid card index" << std::endl;
        return make_discards(is_dealer, gen_crib_stats);
    }
    get_hand().remove_card(get_hand()[i]);
    get_hand().remove_card(get_hand()[j]);
    return {get_hand()[i], get_hand()[j]};
}

RandomPlayer::RandomPlayer(std::string name) : Player(name) {}

Card RandomPlayer::play_card(const CardPile& pile, const Hand& dealt_hand, Card cut) {
    std::vector<Card> playable_cards(get_hand().size());
    for (unsigned int i = 0; i < get_hand().size(); i++) {
        if (pile.can_play_card(get_hand()[i])) {
            playable_cards.push_back(get_hand()[i]);
        }
    }
    if (playable_cards.size() == 0) {
        std::cerr << "No playable cards, this should not happen." << std::endl;
        throw std::runtime_error("No playable cards");
    }
    int i = CribbageRandom::get_instance()->get_int(0, playable_cards.size());
    get_hand().remove_card(get_hand()[i]);
    return get_hand()[i];
}

std::pair<Card, Card> RandomPlayer::make_discards(
  bool is_dealer, GenerateCribStatistics* gen_crib_stats) {
    int i = CribbageRandom::get_instance()->get_int(0, get_hand().size());
    int j = CribbageRandom::get_instance()->get_int(0, get_hand().size() - 1);
    if (j == i)
        j = get_hand().size() - 1;
    get_hand().remove_card(get_hand()[i]);
    return {get_hand()[i], get_hand()[j]};
}

StatPlayer::StatPlayer(std::string name) : Player(name) {}

Card StatPlayer::play_card(const CardPile& pile, const Hand& dealt_hand, Card cut) {
    //TODO: Implement AI

    // Make sure to remove the card from the player's hand
    return get_hand()[0];
}

std::pair<Card, Card> StatPlayer::make_discards(
  bool is_dealer, GenerateCribStatistics* gen_crib_stats) {
    GenerateDiscardStatistics gen_discard(this, is_dealer, gen_crib_stats);
    gen_discard.generate_discard_stats();
    gen_discard.sort_discard_stats(ScoreType::COMBINED, Statistic::MEAN);
    const DiscardStatistics& discard_stats =
      gen_discard.get_best_discard_stats();
    get_hand().remove_card(discard_stats.get_discard1());
    get_hand().remove_card(discard_stats.get_discard2());
    return {discard_stats.get_discard1(), discard_stats.get_discard2()};
}

}  // namespace cribbage
