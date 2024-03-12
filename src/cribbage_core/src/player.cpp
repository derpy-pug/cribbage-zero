#include "player.h"

#include <algorithm>
#include <iostream>

#include "cribbage_random.h"
#include "generate_statistics.h"

namespace cribbage {

PlayerInfo::PlayerInfo(std::string name, PlayerType type)
    : name(name), type(type) {}


Player::Player(PlayerInfo info) : info(info) {}

std::unique_ptr<Player> PlayerFactory::create_player(PlayerInfo info) {
    switch (info.get_type()) {
        case PlayerType::HUMAN:
            return std::make_unique<HumanPlayer>(info);
        case PlayerType::RANDOM:
            return std::make_unique<RandomPlayer>(info);
        case PlayerType::STAT:
            return std::make_unique<StatPlayer>(info);
        default:
            std::cerr << "Invalid player type" << std::endl;
            throw std::runtime_error("Invalid player type");
    }
}

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
    get_hand().remove_card(get_hand()[i]);
    return get_hand()[i];
}

std::pair<Card, Card> HumanPlayer::make_discards(
  bool is_dealer, const CribDiscardProbabilities& crib_discard_probs) {
    if (is_dealer) {
        std::cout << "Your crib" << std::endl;
    } else {
        std::cout << "Opponent's crib" << std::endl;
    }
    std::cout << "Your hand: " << get_hand() << std::endl;
    std::cout << "Discard two cards: ";
    unsigned int i, j;
    // TODO: Check for invalid input
    // Currently, if the user inputs a non-integer, the program will crash
    std::cin >> i;
    std::cin >> j;
    if (i == j || i >= get_hand().size() ||
        j >= get_hand().size()) {
        std::cout << "Invalid card index" << std::endl;
        return make_discards(is_dealer, crib_discard_probs);
    }
    get_hand().remove_card(get_hand()[i]);
    get_hand().remove_card(get_hand()[j]);
    return {get_hand()[i], get_hand()[j]};
}

Card RandomPlayer::play_card(const CardPile& pile, const Hand& dealt_hand, Card cut) {
    std::vector<Card> playable_cards;
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
    Card card = playable_cards[i];
    get_hand().remove_card(card);
    return card;
}

std::pair<Card, Card> RandomPlayer::make_discards(
  bool is_dealer, const CribDiscardProbabilities& crib_discard_probs) {
    int i = CribbageRandom::get_instance()->get_int(0, get_hand().size());
    int j = CribbageRandom::get_instance()->get_int(0, get_hand().size() - 1);
    if (j == i)
        j = get_hand().size() - 1;
    std::pair<Card, Card> pair = {get_hand()[i], get_hand()[j]};
    get_hand().remove_card(get_hand()[i]);
    get_hand().remove_card(get_hand()[j]);
    return pair;
}

Card StatPlayer::play_card(const CardPile& pile, const Hand& dealt_hand, Card cut) {
    //TODO: Implement AI

    std::vector<Card> playable_cards;
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
    Card card = playable_cards[i];
    get_hand().remove_card(card);
    return card;
}

std::pair<Card, Card> StatPlayer::make_discards(
  bool is_dealer, const CribDiscardProbabilities& crib_discard_probs) {
    GenerateDiscardStatistics gen_discard(this, is_dealer, crib_discard_probs);
    gen_discard.generate_all_discard_stats();

    AllDiscardStatistics all_discard_stats = gen_discard.get_all_discard_stats();
    all_discard_stats.sort_discard_stats(ScoreType::COMBINED, Statistic::MEAN);
    const DiscardStatistics& discard_stats =
      all_discard_stats.get_best_discard_stats();
    get_hand().remove_card(discard_stats.get_discard1());
    get_hand().remove_card(discard_stats.get_discard2());
    return {discard_stats.get_discard1(), discard_stats.get_discard2()};
}

}  // namespace cribbage
