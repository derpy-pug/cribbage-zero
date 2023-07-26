#ifndef cards_h
#define cards_h

/*
 * Card
 *   rank: Between 1 - 13 inclusive
 *   	   1 - Ace
 *   	   2 - 2
 *   	    ...
 *   	   11 - Jack
 *   	   12 - Queen
 *   	   13 - King
 *   suit: Between 0 - 3 inclusive
 *   	   0 - Clubs
 *   	   1 - Diamonds
 *   	   2 - Hearts
 *   	   3 - Spades
*/
typedef struct card {
	char rank;
	char suit;
}  Card;

char rank_value(char rank);

typedef struct {
    Card* cards;
    int length;
}  Hand;

typedef struct {
    Card* cards;
    char length;
    char sum_31;
}  CardPile;

Card new_card(char rank, char suit);

void card_to_string(char* card_s, Card card); 

void print_card(Card card);

char compare_card(Card card1, Card card2);


CardPile new_card_pile(Card* cards);

void add_card_to_pile(CardPile* pile, Card card);

char can_add_to_pile(CardPile pile, Card card);

char score_pile_with_card(CardPile pile, Card card);

void reset_pile(CardPile* pile); 

void print_pile(const CardPile pile); 


Hand new_hand(Card* cards, int current_length);

void add_card_to_hand(Hand* hand, Card card);

Card remove_card_from_hand(Hand* hand, int index);

Hand sort_hand_copy(Card cards[], Hand hand);

void sort_hand(Hand* hand);

void print_hand(const Hand hand); 


void new_deck(Card* deck);

void shuffle_deck(Card* deck);

char remove_cards_from_deck(Card* deck, const Hand hand);

// Only shuffles the cards that have been used
// Used for faster shuffling
// used_index is the index of the last card that has been used
void shuffle_used_deck(Card* deck, int num_cards_used);


#endif
