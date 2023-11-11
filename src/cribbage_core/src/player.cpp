#include "player.h"

#include <algorithm>
#include <iostream>

#include "cribbage_random.h"
#include "generate_statistics.h"

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

Card HumanPlayer::play_card() {
    std::cout << "Your hand: " << get_hand() << std::endl;
    std::cout << "Play a card: ";
    int i;
    std::cin >> i;
    return get_hand().begin()[i];
}

std::pair<Card, Card> HumanPlayer::make_discards(
    bool is_my_crib, GenerateCribStatistics* gen_crib_stats) {
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
    return {get_hand()[i], get_hand()[j]};
}

RandomPlayer::RandomPlayer(std::string name) : Player(name) {}

Card RandomPlayer::play_card() {
    int i = CribbageRandom::get_instance()->get_int(0, get_hand().size());
    return get_hand().begin()[i];
}

std::pair<Card, Card> RandomPlayer::make_discards(
    bool is_my_crib, GenerateCribStatistics* gen_crib_stats) {
    Hand discards;
    int i = CribbageRandom::get_instance()->get_int(0, get_hand().size());
    int j = CribbageRandom::get_instance()->get_int(0, get_hand().size() - 1);
    if (j == i)
        j = get_hand().size() - 1;
    return {get_hand()[i], get_hand()[j]};
}

StatPlayer::StatPlayer(std::string name) : Player(name) {}

Card StatPlayer::play_card() {
    //TODO: Implement AI
    return get_hand().begin()[0];
}

std::pair<Card, Card> StatPlayer::make_discards(
    bool is_my_crib, GenerateCribStatistics* gen_crib_stats) {
    GenerateDiscardStatistics gen_discard(this, is_my_crib, gen_crib_stats);
    gen_discard.generate_discard_stats();
    gen_discard.sort_discard_stats(ScoreType::COMBINED, Statistic::MEAN);
    const DiscardStatistics& discard_stats =
        gen_discard.get_best_discard_stats();
    //TODO: Implement AI
    return {discard_stats.get_discard1(), discard_stats.get_discard2()};
}
