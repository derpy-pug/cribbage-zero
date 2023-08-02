#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cards.h"
#include "cribbage_scoring.h"

char compare_card(Card card1, Card card2) {
    if (card1.rank > card2.rank) return 1;
    if (card1.rank < card2.rank) return -1;
    return 0;
} 

void card_to_string(char* card_s, Card card) {
    static const char suits[4] = "CDHS";
    static const char ranks[14] = "0A23456789TJQK";
   sprintf(card_s, "%c%c", ranks[(int)card.rank], suits[(int)card.suit]); 
}

void print_card(const Card card) {
    char card_s[3];
    card_to_string(card_s, card);
    printf("%s", card_s);
}

void print_hand(const Hand hand) {
    for (int i = 0; i < hand.length; i++) {
        char card_s[3];
        card_to_string(card_s, hand.cards[i]);
        printf("%s ", card_s);
    }
}

void print_pile(const CardPile pile) {
    for (int i = 0; i < pile.length; i++) {
        char card_s[3];
        card_to_string(card_s, pile.cards[i]);
        printf("%s ", card_s);
    }
}

char rank_value(char rank) {
    if (rank > 10) return 10;
    return rank;
}

CardPile init_card_pile(Card* cards) {
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

char can_add_to_pile(CardPile pile, Card card) {
    return pile.sum_31 + rank_value(card.rank) <= 31;
}

void remove_last_card_from_pile(CardPile* pile) {
    if (pile->length == 0) return;
    pile->length--;
    pile->sum_31 -= rank_value(pile->cards[(int)pile->length].rank);
}

char score_pile_with_card(CardPile pile, Card card) {
    if (can_add_to_pile(pile, card)) {
        add_card_to_pile(&pile, card);
        int score = score_pile(pile);
        remove_last_card_from_pile(&pile);
        return score;
    }
    return -1;    
}

void reset_pile(CardPile* pile) {
    pile->length = 0;
    pile->sum_31 = 0;
}

Card init_card(char rank, char suit) {
	Card card;
	card.rank = rank;
	card.suit = suit;
	return card;
}

Hand init_hand(Card* cards, int current_length) {
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

void init_deck(Card* deck) {
	for (int suit = 0; suit < 4; suit++) {
		for (int rank = 1; rank < 14; rank++) {
			int i = 13 * suit + (rank - 1);
			deck[i] = init_card(rank, suit);
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

char remove_cards_from_deck(Card* deck, const Hand hand) {
    // remove cards in hand from deck
    Card _hand_cpy[6];
    Hand hand_cpy = {_hand_cpy, 0}; 
    for (int i = 0; i < 6; i++) {
        add_card_to_hand(&hand_cpy, hand.cards[i]);
    }
    int num_cards_removed = 0;
    int deck_last = 51;
    for (int i = 0; i < 52; i++) {
        for (int j = 0; j < hand_cpy.length; j++) {
            if (compare_card(deck[i], hand_cpy.cards[j]) == 0) {
                deck[i] = deck[deck_last--];
                remove_card_from_hand(&hand_cpy, j);
                num_cards_removed++;
                break;
            }
        }
    }
    return num_cards_removed;
}

char remove_cards_from_deck_keep_order(Card* deck, const Hand hand) {
    // remove cards in hand from deck
    Card _hand_cpy[6];
    Hand hand_cpy = {_hand_cpy, 0}; 
    for (int i = 0; i < 6; i++) {
        add_card_to_hand(&hand_cpy, hand.cards[i]);
    }
    int num_cards_removed = 0;
    int deck_last = 51;
    for (int i = 0; i <= deck_last; i++) {
        for (int j = 0; j < hand_cpy.length; j++) {
            if (compare_card(deck[i], hand_cpy.cards[j]) == 0) {
                memmove(&deck[i], &deck[i + 1], sizeof(Card) * (deck_last - i));
                remove_card_from_hand(&hand_cpy, j);
                num_cards_removed++;
                deck_last--;
                break;
            }
        }
    }
    return num_cards_removed;
}
