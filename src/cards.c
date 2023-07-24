#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cards.h"

void card_to_string(char* card_s, Card card) {
    static const char suits[4] = "CDHS";
    static const char ranks[14] = "0123456789TJQK";
   sprintf(card_s, "%c%c", ranks[(int)card.rank], suits[(int)card.suit]); 
}

void print_card(const Card card) {
    char card_s[3];
    card_to_string(card_s, card);
    printf("%s\n", card_s);
}

void print_hand(const Hand hand) {
    for (int i = 0; i < hand.length; i++) {
        char card_s[3];
        card_to_string(card_s, hand.cards[i]);
        printf("%s ", card_s);
    }
    printf("\n");
}

void print_pile(const CardPile pile) {
    for (int i = 0; i < pile.length; i++) {
        char card_s[3];
        card_to_string(card_s, pile.cards[i]);
        printf("%s ", card_s);
    }
    printf("\n");
}

char rank_value(char rank) {
    if (rank > 10) return 10;
    return rank;
}

CardPile new_card_pile(Card* cards) {
    CardPile pile;
    pile.cards = cards;
    pile.length = 0;
    pile.sum_31 = 0;
    return pile;
}

void add_card_to_pile(CardPile* pile, Card card) {
    pile->cards[(int)pile->length] = card;
    pile->sum_31 += rank_value(card.rank);
    pile->length++;
}

void reset_pile(CardPile* pile) {
    pile->length = 0;
    pile->sum_31 = 0;
}

Card new_card(char rank, char suit) {
	Card card;
	card.rank = rank;
	card.suit = suit;
	return card;
}

Hand new_hand(Card* cards, int current_length) {
    Hand hand;
    hand.cards = cards;
    hand.length = current_length;
    return hand;
}

void add_card_to_hand(Hand* hand, Card card) {
    hand->cards[hand->length] = card;
    hand->length++;
}

Card remove_card_from_hand(Hand* hand, int index) {
    Card card = hand->cards[index];
    hand->cards[index] = hand->cards[hand->length - 1];
    hand->length--;
    return card;
}

int compare_cards_rank(const void* a, const void* b) {
    Card* card_a = (Card*)a;
    Card* card_b = (Card*)b;
    return card_a->rank - card_b->rank;
}

Hand sort_hand_copy(Card cards[], Hand hand) {
    memcpy(cards, hand.cards, sizeof(Card) * hand.length);
    hand.cards = cards;
    qsort(hand.cards, hand.length, sizeof(Card), compare_cards_rank);
    return hand;
}

void sort_hand(Hand *hand) {
    qsort(hand->cards, hand->length, sizeof(Card), compare_cards_rank);
}

void new_deck(Card* deck) {
	for (int suit = 0; suit < 4; suit++) {
		for (int rank = 1; rank < 14; rank++) {
			int i = 13 * suit + (rank - 1);
			deck[i] = new_card(rank, suit);
		}
	}
}

void shuffle_deck(Card* deck) {
	// Modern Fisher-Yates
	for (int i = 51; i > 0; i--) {
		int j = rand() % (i + 1);
		Card temp = deck[j];
		deck[j] = deck[i];
		deck[i] = temp;
	}
}

void shuffle_used_deck(Card *deck, int num_cards_used) {
	// Modern Fisher-Yates Reversed

	for (int i = 0; i < num_cards_used; i++) {
		int j = rand() % (52 - i) + i;
		Card temp = deck[j];
		deck[j] = deck[i];
		deck[i] = temp;
	}
}

