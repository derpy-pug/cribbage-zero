#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H


#include "cards.h"
#include "player.h"

// Returns an index of the card that the player wants to play
int get_player_play_input(const Hand hand, const CardPile pile, const PlayerInfo* player_info, Card cut);

// Returns the indicies of the cards that the player wants to discard
// in the discards array
void get_player_discards_input(const Hand hand, int discards[2], const PlayerInfo* player_info, char is_my_crib);
#endif
