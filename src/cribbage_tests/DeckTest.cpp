#include <gtest/gtest.h>
#include "deck.h"
#include <map>
#include <utility>
#include <array>
#include <algorithm>

using namespace cribbage;


TEST(DeckTest, Initialization) {
    Deck deck;
    EXPECT_EQ(deck.size(), 52);
    for (size_t i = 0; i < deck.size(); ++i) {
        EXPECT_EQ(deck.get_cards()[i].get_rank_int(), (i % 13) + 1);
        EXPECT_EQ(deck.get_cards()[i].get_suit_int(), (i / 13));
    }

    deck.shuffle();
    deck.deal_hand(10);
    deck.deal_card();
    deck.make_deck();
    EXPECT_EQ(deck.size(), 52);
    for (size_t i = 0; i < deck.size(); ++i) {
        EXPECT_EQ(deck.get_cards()[i].get_rank_int(), (i % 13) + 1);
        EXPECT_EQ(deck.get_cards()[i].get_suit_int(), (i / 13));
    }
}

TEST(DeckTest, Shuffle) {
    // Statistical test
    const int shuffleCount = 1000;
    struct CardComparator {
    bool operator()(const std::pair<int, cribbage::Card>& lhs, const std::pair<int, cribbage::Card>& rhs) const {
        // Compare first by the integer position
        if (lhs.first != rhs.first) return lhs.first < rhs.first;
        // Then compare by card rank and suit
        if (lhs.second.get_rank() != rhs.second.get_rank()) return lhs.second.get_rank() < rhs.second.get_rank();
        return lhs.second.get_suit() < rhs.second.get_suit();
    }
    };
    std::map<std::pair<size_t, Card>, size_t, CardComparator> cardPositionCount;

    Deck deck;
    deck.shuffle();
    for (size_t i = 0; i < shuffleCount; ++i) {
        /* deck.deal_hand(23); */
        /* deck.shuffleTopCardsIntoDeck(); */
        deck.shuffle();
        auto shuffledDeck = deck.get_cards();

        // Track the position of each card across all shuffles
        for (size_t pos = 0; pos < shuffledDeck.size(); ++pos) {
            Card card = shuffledDeck[pos];
            cardPositionCount[{pos, shuffledDeck[pos]}]++;
        }
    }

    // For debugging
    // Print the distribution of card positions

    /* std::array<size_t, 52> positions_distribution; */
    /* for (Card card : deck.get_cards()) { */
    /*     positions_distribution.fill(0); */
    /*     for (const auto& entry : cardPositionCount) { */
    /*         if (entry.first.second == card) { */
    /*             positions_distribution[entry.first.first] = entry.second; */
    /*         } */
    /*     } */
    /*     std::cout << "Card positions distribution (" << card << "): "; */
    /*     for (size_t i = 0; i < positions_distribution.size(); ++i) { */
    /*         std::cout << positions_distribution[i] << " "; */
    /*     } */
    /*     std::cout << std::endl; */
    /* } */


    // Perform some analysis (e.g., ensure no card stays in the same position too often)
    for (const auto& entry : cardPositionCount) {
        int position = entry.first.first;
        const Card& card = entry.first.second;
        int count = entry.second;

        EXPECT_NEAR(count, (float)shuffleCount / (int)deck.size(), shuffleCount * 0.02) << "Card " << card << " appears in position " << position << " too frequently!";
    }
}

TEST(DeckTest, ShuffleTopCardsIntoDeck) {
    // Statistical test
    const int shuffleCount = 1000;
    const int numCardsToShuffle = 10;
    struct CardComparator {
    bool operator()(const std::pair<int, cribbage::Card>& lhs, const std::pair<int, cribbage::Card>& rhs) const {
        // Compare first by the integer position
        if (lhs.first != rhs.first) return lhs.first < rhs.first;
        // Then compare by card rank and suit
        if (lhs.second.get_rank() != rhs.second.get_rank()) return lhs.second.get_rank() < rhs.second.get_rank();
        return lhs.second.get_suit() < rhs.second.get_suit();
    }
    };
    std::map<std::pair<size_t, size_t>, size_t> cardPositionCount;


    Deck deck;
    deck.shuffle();
    for (size_t i = 0; i < shuffleCount; ++i) {
        Hand hand = deck.deal_hand(numCardsToShuffle);
        deck.shuffleTopCardsIntoDeck();
        auto shuffledDeck = deck.get_cards();

        // Track the position of each card across all shuffles
        for (size_t pos = 0; pos < shuffledDeck.size(); ++pos) {
            size_t pos_hand = std::find(hand.get_cards().begin(), hand.get_cards().end(), shuffledDeck[pos]) - hand.get_cards().begin();
            if (pos_hand < hand.size()) {
                cardPositionCount[{pos_hand, pos}]++;
            }
        }
    }

    // For debugging
    // Print the distribution of card positions

    /* std::array<size_t, numCardsToShuffle> positions_distribution; */
    /* for (size_t i = 0; i < numCardsToShuffle; ++i) { */
    /*     positions_distribution.fill(0); */
    /*     for (const auto& entry : cardPositionCount) { */
    /*         if (entry.first.second == i) { */
    /*             positions_distribution[entry.first.first] = entry.second; */
    /*         } */
    /*     } */
    /*     std::cout << "Card positions distribution (" << i << "): "; */
    /*     for (size_t i = 0; i < positions_distribution.size(); ++i) { */
    /*         std::cout << positions_distribution[i] << " "; */
    /*     } */
    /*     std::cout << std::endl; */
    /* } */


    // Perform some analysis (e.g., ensure no card stays in the same position too often)
    for (const auto& entry : cardPositionCount) {
        int pos_hand = entry.first.first;
        int pos = entry.first.second;
        int count = entry.second;

        EXPECT_NEAR(count, (float)shuffleCount / (int)deck.size(), shuffleCount * 0.02) << "Card " << pos << " appears in position " << pos_hand << " too frequently!";
    }
}

TEST(DeckTest, DealCard) {
    Deck deck;
    deck.shuffle();
    EXPECT_EQ(deck.size(), 52);
    for (size_t i = 0; i < deck.size(); ++i) {
        Card card = deck.deal_card();
        EXPECT_EQ(deck.cards_remaining(), 52 - i - 1);
    }
    EXPECT_EQ(deck.cards_remaining(), 0);
    Card card = deck.deal_card();
    EXPECT_EQ(card.get_rank(), Rank::NONE);
    EXPECT_EQ(card.get_suit(), Suit::NONE);
    EXPECT_EQ(deck.cards_remaining(), 0);
}

TEST(DeckTest, DealCardAndRemove) {
    Deck deck;
    deck.shuffle();
    size_t deckSize = deck.size();
    EXPECT_EQ(deck.size(), 52);
    EXPECT_EQ(deck.cards_remaining(), 52);
    for (size_t i = 0; i < deckSize; ++i) {
        Card card = deck.deal_card_and_remove();
        EXPECT_EQ(deck.cards_remaining(), 52 - i - 1);
        EXPECT_EQ(deck.size(), 52 - i - 1);
    }
    EXPECT_EQ(deck.cards_remaining(), 0);
    Card card = deck.deal_card_and_remove();
    EXPECT_EQ(card.get_rank(), Rank::NONE);
    EXPECT_EQ(card.get_suit(), Suit::NONE);
    EXPECT_EQ(deck.cards_remaining(), 0);

    deck.make_deck();
    deck.shuffle();
    EXPECT_EQ(deck.size(), 52);
    EXPECT_EQ(deck.cards_remaining(), 52);
    Hand hand;
    for( size_t i = 0; i < 10; ++i) {
        Card card = deck.deal_card_and_remove();
        hand.add_card(card);
        EXPECT_EQ(deck.cards_remaining(), 52 - i - 1);
        EXPECT_EQ(deck.size(), 52 - i - 1);
    }
    deck.shuffle();
    EXPECT_EQ(deck.size(), 42);
    EXPECT_EQ(deck.cards_remaining(), 42);
    // Check that the hand is not in the deck
    for (Card card : hand.get_cards()) {
        EXPECT_EQ(std::find(deck.get_cards().begin(), deck.get_cards().end(), card), deck.get_cards().end());
    }
}

TEST(DeckTest, DealHand) {
    Deck deck;
    deck.shuffle();
    Hand hand = deck.deal_hand(10);
    EXPECT_EQ(hand.size(), 10);
    EXPECT_EQ(deck.cards_remaining(), 42);
    EXPECT_EQ(deck.size(), 52);

    deck.shuffle();
    EXPECT_EQ(deck.size(), 52);
    EXPECT_EQ(deck.cards_remaining(), 52);
    hand = deck.deal_hand(52);
    EXPECT_EQ(hand.size(), 52);
    EXPECT_EQ(deck.cards_remaining(), 0);
    EXPECT_EQ(deck.size(), 52);
}

TEST(DeckTest, DealHandAndRemove) {
    Deck deck;
    deck.shuffle();
    Hand hand = deck.deal_hand_and_remove(10);
    EXPECT_EQ(hand.size(), 10);
    EXPECT_EQ(deck.cards_remaining(), 42);
    EXPECT_EQ(deck.size(), 42);

    for (Card card : hand.get_cards()) {
        EXPECT_EQ(std::find(deck.get_cards().begin(), deck.get_cards().end(), card), deck.get_cards().end());
    }

    deck.shuffle();
    EXPECT_EQ(deck.size(), 42);
    EXPECT_EQ(deck.cards_remaining(), 42);
    hand = deck.deal_hand_and_remove(42);
    EXPECT_EQ(hand.size(), 42);
    EXPECT_EQ(deck.cards_remaining(), 0);
    EXPECT_EQ(deck.size(), 0);

    deck.make_deck();
    EXPECT_EQ(deck.size(), 52);
    hand = deck.deal_hand_and_remove(100);
    EXPECT_EQ(hand.size(), 52);
    EXPECT_EQ(deck.cards_remaining(), 0);
    EXPECT_EQ(deck.size(), 0);


}
