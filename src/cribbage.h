#ifndef CRIBBAGE_H
#define CRIBBAGE_H
#include "cards.h"
#include "player.h"

// Enum for if there is a winner
typedef enum {
    NO_WINNER,
    DEALER,
    PONE,
} Winner;

Winner cribbage_game(Card* deck, PlayerInfo* player1, PlayerInfo* player2); 

void discard_turn(Hand* hand, Hand* crib, PlayerInfo* player, char is_my_crib);
#endif
