#include <gtest/gtest.h>
#include "card.h"
#include <array>

using namespace cribbage;

TEST(CardTest, InitializationAndGetters) {
    Card card;
    EXPECT_EQ(card.get_rank(), Rank::NONE);
    EXPECT_EQ(card.get_suit(), Suit::NONE);

    for (int i = 1; i <= 13; i++) {
        for (int j = 0; j < 4; j++) {
            Suit suit = static_cast<Suit>(j);
            Rank rank = static_cast<Rank>(i);
            Card card{suit, rank};
            EXPECT_EQ(card.get_rank(), rank);
            EXPECT_EQ(card.get_suit(), suit);
        }
    }
    
    std::array<char, 5> suits = {'C', 'D', 'H', 'S', 'N'};
    std::array<char, 14> ranks = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'N'};

    for (char suit : suits) {
        for (char rank : ranks) {
            std::string card_str = std::string{rank} + std::string{suit};
            card = Card{card_str};
            EXPECT_EQ(card.to_string(), card_str);
        }
    }
}

TEST(CardTest, OtherGetters) {
    for (int i = 1; i <= 13; i++) {
        for (int j = 0; j < 4; j++) {
            Suit suit = static_cast<Suit>(j);
            Rank rank = static_cast<Rank>(i);
            Card card{suit, rank};
            EXPECT_EQ(card.get_rank_int(), i);
            EXPECT_EQ(card.get_suit_int(), j);
        }
    }

    for (int i = 1; i <= 13; i++) {
        Card card{Suit::CLUBS, static_cast<Rank>(i)};
        int value = card.get_value();
        if (card.get_rank_int() > 10) {
            EXPECT_EQ(value, 10);
        } else {
            EXPECT_EQ(value, i);
        }
    }
}

TEST(CardTest, Comparison) {
    Card card1{Suit::CLUBS, Rank::ACE};
    Card card2{Suit::CLUBS, Rank::TWO};
    Card card3{Suit::DIAMONDS, Rank::ACE};
    Card card4{Suit::DIAMONDS, Rank::TWO};
    Card card5{Suit::HEARTS, Rank::THREE};
    Card card6{Suit::SPADES, Rank::THREE};
    Card card9{Suit::CLUBS, Rank::TWO};
    Card cardNN{Suit::NONE, Rank::NONE};
    Card cardNN2{Suit::NONE, Rank::NONE};

    EXPECT_TRUE(Card::compare_rank(card1, card2));
    EXPECT_FALSE(Card::compare_rank(card1, card3));
    EXPECT_FALSE(Card::compare_rank(card2, card3));
    EXPECT_FALSE(Card::compare_rank(card2, card4));
    EXPECT_FALSE(Card::compare_rank(card5, card6));
    EXPECT_TRUE(Card::compare_rank(card1, card5));

    EXPECT_TRUE(Card::compare_suit(card1, card3));
    EXPECT_FALSE(Card::compare_suit(card1, card2));
    EXPECT_TRUE(Card::compare_suit(card2, card4));
    EXPECT_TRUE(Card::compare_suit(card5, card6));
    EXPECT_FALSE(Card::compare_suit(card6, card5));
    EXPECT_TRUE(Card::compare_suit(card1, card5));

    EXPECT_TRUE(card2 == card9);
    EXPECT_FALSE(card1 == card2);
    EXPECT_FALSE(card1 == card3);
    EXPECT_FALSE(card1 == card5);
    EXPECT_TRUE(cardNN == cardNN2);
}
