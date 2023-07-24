#ifndef CRIBBAGE_H
#define CRIBBAGE_H
#include "cards.h"
#include "player.h"
#include "random.h"
#include "player.h"

// Enum for if there is a winner
typedef enum {
    NO_WINNER,
    DEALER,
    PONE,
} Winner;

Winner cribbage_game(Card* deck, PlayerInfo* player1, PlayerInfo* player2); 
#endif
