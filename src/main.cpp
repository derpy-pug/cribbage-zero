#include <iostream>

#include "cribbage.h"

int main(int argc, char** argv) {
    /* if (argc != 2) { */
    /*     std::cout << "Usage: " << argv[0] << " <name>" << std::endl; */
    /*     return 1; */
    /* } */
    /* std::cout << "Hello, " << argv[1] << "!" << std::endl; */

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
