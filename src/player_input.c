#include "player_input.h"
#include "cards.h"
#include "utils/random.h"
#include <stdio.h>
#include <string.h>
#include "discard.h"


int player_play_input_human(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    printf("\n");
    printf("Cut: ");
    print_card(cut);
    printf("\n");
    printf("Pile: ");
    print_pile(pile);
    printf("\n");
    printf("Hand: ");
    print_hand(hand);
    printf("\n");

    printf("Enter card index: ");
    char input[3];
    if (fgets(input, 3, stdin) == input) {
        if (strchr(input, '\n') == NULL) {
            char c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("Input too long\n");
            return -1;
        }
    } else {
        printf("Error reading input\n");
        return -1;
    }
    int card_index;
    sscanf(input, "%1d", &card_index);
    --card_index;
    return card_index;
}

int player_play_input_random(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    int card_index = random_int(hand.length); 
    while (!can_add_to_pile(pile, hand.cards[card_index])) {
        card_index = random_int(hand.length);
    }
    return card_index;
}

char player_play_input_ai(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    Card _pile[8];
    CardPile pile_copy = pile;
    pile_copy.cards = _pile;
    memcpy(pile_copy.cards, pile.cards, pile.length * sizeof(Card));

    int max_score = -1;
    int max_score_index;
    for (int i = 0; i < hand.length; i++) {
        char score = score_pile_with_card(pile_copy, hand.cards[i]);
        if (score == -1) continue;
        if (score > max_score) {
            max_score = score;
            max_score_index = i;
        }
    }
    return max_score_index;
}

int get_player_play_input_ai(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    return player_play_input_ai(hand, pile, player_info, cut);
}


int get_player_play_input(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    switch (player_info->type) {
        case HUMAN:
            return player_play_input_human(hand, pile, player_info, cut);
        case RANDOM:
            return player_play_input_random(hand, pile, player_info, cut);
        case AI:
            return get_player_play_input_ai(hand, pile, player_info, cut);
    }
    return -1;
}

void get_player_discards_input_human(const Hand hand, int discards[2], const PlayerInfo* player_info, char is_my_crib) {
    printf("\n");
    is_my_crib ? printf("My crib\n") : printf("Opponent's crib\n");
    printf("Hand: ");
    print_hand(hand);
    printf("\n");

    printf("Enter 2 cards to discard: ");
    char input[5];
    if (fgets(input, 5, stdin) == input) {
        if (strchr(input, '\n') == NULL) {
            char c;
            while ((c = getchar()) != '\n' && c != EOF) { }
            printf("Input too long\n");
            return;
        }
    } else {
        printf("Error reading input\n");
        return;
    }
    sscanf(input, "%1d %1d", &discards[0], &discards[1]);
    discards[0]--;
    discards[1]--;
}

void get_player_discards_input_random(const Hand hand, int discards[2], const PlayerInfo* player_info, char is_my_crib) {
    discards[0] = random_int(hand.length);
    discards[1] = random_int(hand.length);
    while (discards[0] == discards[1]) {
        discards[1] = random_int(hand.length);
    }
}

void get_player_discards_input_ai(const Hand hand, int discards[2], const PlayerInfo* player_info, char is_my_crib) {
    // TODO
    DiscardStats stats = best_discard(hand, is_my_crib);
    discards[0] = stats.discards.first;
    discards[1] = stats.discards.second;
}

void get_player_discards_input(const Hand hand, int discards[2], const PlayerInfo* player_info, char is_my_crib) {
	// Force user to discard 2 unique cards
    // TODO
    switch (player_info->type) {
        case HUMAN:
            get_player_discards_input_human(hand, discards, player_info, is_my_crib);
            break;
        case RANDOM:
            get_player_discards_input_random(hand, discards, player_info, is_my_crib);
            break;
        case AI:
            get_player_discards_input_ai(hand, discards, player_info, is_my_crib);
            break;
    }
}
