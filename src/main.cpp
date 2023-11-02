#include <iostream>

#include "cribbage.h"
#include "generate_crib_statistics.h"

void test_hand_deck() {
    Hand hand;
    Deck deck;
    deck.shuffle();
    hand.add_card(deck.deal_card());
    hand.add_card(deck.deal_card());
    hand.add_card(deck.deal_card());
    hand.add_card(deck.deal_card());
    hand.sort();
    Card cut = deck.deal_card();
    std::cout << "Hand: "<< hand << std::endl;
    std::cout << "Cut: " << cut << std::endl;
    std::cout << "Score: " << score_hand(hand, cut, false) << std::endl;
}

void test_hand_error_deck() {

    Hand hand;
    hand.add_card(Card(Suit::CLUBS, Rank::THREE));
    hand.add_card(Card(Suit::HEARTS, Rank::FOUR));
    hand.add_card(Card(Suit::HEARTS, Rank::EIGHT));
    hand.add_card(Card(Suit::SPADES, Rank::JACK));
    hand.sort();
    Card cut = Card(Suit::HEARTS, Rank::QUEEN);
    std::cout << "Hand: "<< hand << std::endl;
    std::cout << "Cut: " << cut << std::endl;
    std::cout << "Score: " << score_hand(hand, cut, false) << std::endl;
}

int main(int argc, char** argv) {
    /* if (argc != 2) { */
    /*     std::cout << "Usage: " << argv[0] << " <name>" << std::endl; */
    /*     return 1; */
    /* } */
    /* std::cout << "Hello, " << argv[1] << "!" << std::endl; */

    Player* p1 = new RandomPlayer("Randy");
    Player* p2 = new RandomPlayer("Rando");
    
    GenerateCribStatistics gen_stats(p1, p2);
    gen_stats.generate_all_tables();
    gen_stats.save_freq_tables("freq_tables.txt");
    gen_stats.save_mean_tables("mean_tables.txt");
    gen_stats.save_max_min_tables("tables.txt");
    gen_stats.save_std_dev_tables("std_dev_tables.txt");
    gen_stats.save_variance_tables("variance_tables.txt");
    gen_stats.save_median_tables("median_tables.txt");

    /* test_hand_error_deck(); */
}
