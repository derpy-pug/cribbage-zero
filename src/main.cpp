#include <iostream>

#include "cribbage.h"
#include "generate_statistics.h"

#define TABLE_DIR std::string("../tables/")

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

void test_hand() {

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
    
    Deck deck;
    deck.shuffle();
    Hand hand1 = deck.deal_hand(6);
    hand1.sort();
    Hand hand2 = deck.deal_hand(6);
    hand2.sort();

    Player* p1 = new StatPlayer("Staples");
    p1->set_hand(hand1);
    Player* p2 = new StatPlayer("Stanly");
    p2->set_hand(hand2);
    
    GenerateCribStatistics gen_stats(p1, p2);
    std::string dirname = TABLE_DIR + std::string("stat_vs_stat/");
    gen_stats.generate_all_tables();
    gen_stats.save_tables(dirname);
    //gen_stats.load_tables(dirname);

    std::cout << "Hand: " << hand1 << std::endl;
    GenerateDiscardStatistics gen_discard(p1, false, &gen_stats);
    gen_discard.generate_discard_stats();
    gen_discard.sort_discard_stats(ScoreType::COMBINED, Statistic::MEAN);
    const DiscardStatistics& discard_stats = gen_discard.get_best_discard_stats();
    //std::cout << discard_stats << std::endl;

    std::cout << gen_discard << std::endl;

}
