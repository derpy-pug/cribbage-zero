#include "player_input.h"
#include "cards.h"
#include "random.h"
#include <stdio.h>
#include <string.h>


int human_input(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    printf("\n");
    printf("Cut: ");
    print_card(cut);
    printf("Pile: ");
    print_pile(pile);
    printf("Hand: ");
    print_hand(hand);

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

int random_input(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    return random_int(hand.length);
}

char probability_ai(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    // TODO
    return 0;
}

int ai_input(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    return probability_ai(hand, pile, player_info, cut);
}


int get_player_play_input(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut) {
    switch (player_info->type) {
        case HUMAN:
            return human_input(hand, pile, player_info, cut);
        case RANDOM:
            return random_input(hand, pile, player_info, cut);
        case AI:
            return ai_input(hand, pile, player_info, cut);
    }
    return -1;
}

void get_player_discards_input_human(const Hand hand, int discards[2], const PlayerInfo* player_info, char is_my_crib) {
    printf("\n");
    is_my_crib ? printf("My crib\n") : printf("Opponent's crib\n");
    printf("Hand: ");
    print_hand(hand);

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
