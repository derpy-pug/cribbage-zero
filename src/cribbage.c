#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player_input.h"
#include "cards.h"
#include "cribbage.h"
#include "cribbage_scoring.h"
#include "player_input.h"


// Requires discards to have initialized values
// and values to be in range [0, 5]
//
// Removes the cards from the hand and adds them to the crib
void discard_into_crib(Hand* hand, Hand* crib, const int discards[2]) {
    // Interating through the hand backwards to avoid index issues
    // since we are removing cards while iterating
    for (int i = hand->length - 1; i >= 0; i--) {
        if (i == discards[0] || i == discards[1]) {
            add_card_to_hand(crib, hand->cards[i]);
            remove_card_from_hand(hand, i);
        }
    }
}

// Can sort the hand if the player is human
// TODO: Pass in the two cards that were discarded
void discard_turn(Hand* hand, Hand* crib, PlayerInfo* player, char is_my_crib) {
    if (player->type == HUMAN) sort_hand(hand);
    int discards[2];
    while (1) {
        discards[0] = -1;
        discards[1] = -1;
        get_player_discards_input(*hand, discards, player, is_my_crib);
        if (discards[0] < hand->length && discards[1] < hand->length) {
            if (discards[0] != discards[1]) {
                break;
            }
            if (player->type == HUMAN) printf("Invalid discards: Duplicate card\n");
            continue;
        }
        if (player->type == HUMAN) printf("Invalid discards: Index out of bounds\n");
    }
    discard_into_crib(hand, crib, discards);
}

// turn: 0 -> player 1, 1 -> player 2
// Requires hand to have at least 1 card
// Return Card.rank == 0 if go
char play_turn(Hand hand, CardPile pile, PlayerInfo* player, Card cut) {
    unsigned char card_index;
    while (1) {
        card_index = get_player_play_input(hand, pile, player, cut);
        if (card_index < hand.length) {
            if (pile.sum_31 + rank_value(hand.cards[card_index].rank) > 31) {
                if (player->type == HUMAN) 
                    printf("Invalid card: Sum over 31\n");
                continue;
            }
            break;
        }
        if (player->type == HUMAN)
            printf("Invalid card: Index out of bounds\n");
    }
    return card_index;
}

typedef enum {
    NO_GO,
    GO,
    GO_TWICE
}GoState;
GoState get_go_state(const Hand hand, const CardPile pile) {
    for (int i = 0; i < hand.length; i++) {
        char is_under_31 = pile.sum_31 + rank_value(hand.cards[i].rank) <= 31;
        if(is_under_31) return NO_GO;
    }
    return GO;
}

Winner play_phase(const Hand hand1, const Hand hand2, PlayerInfo* dealer, PlayerInfo* pone, Card cut) {
    Card _hand1_played[4];
    Card _hand2_played[4];
    memcpy(_hand1_played, hand1.cards, 4 * sizeof(Card));
    memcpy(_hand2_played, hand2.cards, 4 * sizeof(Card));
    Hand dealer_hand = new_hand(_hand1_played, 4);
    Hand pone_hand = new_hand(_hand2_played, 4);

    // Keep all played cards for history
    //Card _all_played_cards[8];

    Card _pile[8]; 
    CardPile pile = new_card_pile(_pile);

    GoState go = NO_GO;
    char turn = 0;
    char are_cards_left = dealer_hand.length || pone_hand.length;
    while (are_cards_left || go != GO_TWICE) {
        are_cards_left = dealer_hand.length || pone_hand.length;
        go += get_go_state(turn ? pone_hand : dealer_hand, pile);
        if (go == NO_GO) {
            // TODO combine hand and player in one struct
            Hand* hand = turn ? &pone_hand : &dealer_hand;
            PlayerInfo* player = turn ? pone : dealer;
            char index = play_turn(*hand, pile, player, cut);
            Card card = remove_card_from_hand(hand, index);
            add_card_to_pile(&pile, card);
            
            // Score the pile
            char pile_score = score_pile(pile);
            int* score = turn ? &pone->score : &dealer->score;
            *score += pile_score;
            if (*score >= 121) return turn ? PONE : DEALER;
        }
        // GO does nothing
        else if (go == GO_TWICE) {
            // This turns player gets 1 point for go
            PlayerInfo* player = turn ? pone : dealer;
            player->score += 1;
            if (player->score >= 121) return turn ? PONE : DEALER;

            reset_pile(&pile); 
            // When there are no cards left, go = GO_TWICE and the round ends
            if (are_cards_left) go = NO_GO;
        } 
        turn = !turn;
    } 
    return NO_WINNER;
}

Winner cribbage_round(const Card* deck, PlayerInfo* dealer, PlayerInfo* pone) {
    Card deckcpy[17];
    memcpy(deckcpy, deck, 13 * sizeof(Card));

    Hand dealer_hand = new_hand(deckcpy, 6);
    Hand pone_hand = new_hand(deckcpy + 6, 6);
    Hand crib = new_hand(deckcpy + 13, 0);
	Card cut = deckcpy[12];

    // Check for his heels
    dealer->score += cut.rank == 11 ? 2 : 0;
    if (dealer->score >= 121) return DEALER;

    // Discard phase
    discard_turn(&dealer_hand, &crib, dealer, 1);
    discard_turn(&pone_hand, &crib, pone, 0);
    

    Winner winner = play_phase(dealer_hand, pone_hand, dealer, pone, cut);
    if (winner != NO_WINNER) { 
        return winner;
    }

    // Score Phase
    if (dealer->type == HUMAN || pone->type == HUMAN) {
        printf("\nScore Phase\n");
        printf("Cut: ");
        print_card(cut);
        printf("\n");
    }

    int score = score_hand(pone_hand, cut, 0);
    pone->score += score;
    if (dealer->type == HUMAN || pone->type == HUMAN) {
        printf("%s's Hand: ", pone->name);
        sort_hand(&pone_hand);
        print_hand(pone_hand);
        printf("Score: %d\n", score);
    }
    if (pone->score >= 121) return PONE;

    score = score_hand(dealer_hand, cut, 0);
    dealer->score += score;
    if (dealer->type == HUMAN || pone->type == HUMAN) {
        printf("%s's Hand: ", dealer->name);
        sort_hand(&dealer_hand);
        print_hand(dealer_hand);
        printf("Score: %d\n", score);
    }

    score = score_hand(crib, cut, 0);
    dealer->score += score;
    if (dealer->type == HUMAN || pone->type == HUMAN) {
        printf("%s's Crib: ", dealer->name);
        sort_hand(&crib);
        print_hand(crib);
        printf("Score: %d\n", score);
    }
    if (dealer->score >= 121) return DEALER;

	return NO_WINNER;
}

Winner cribbage_game(Card* deck, PlayerInfo* player1, PlayerInfo* player2) {
    int turn = 0;
    int round = 1;
    shuffle_deck(deck);
    int i = 0;
    while (i++ < 100) {
        shuffle_used_deck(deck, 13);
        PlayerInfo* dealer = turn ? player2 : player1;
        PlayerInfo* pone = turn ? player1 : player2;
        if (dealer->type == HUMAN || pone->type == HUMAN) {
            printf("\n\n(%d) %s's Score -> %d  - %s's Score ->  %d\n", 
                    round, dealer->name, dealer->score, pone->name, pone->score);
        }
        Winner winner = cribbage_round(deck, dealer, pone);
        if (winner != NO_WINNER) {
            if (turn == 0) return winner;
            else return winner == DEALER ? PONE : DEALER;
        }
        turn = !turn;
        round++;
    }
    return NO_WINNER;
}


