#ifndef DISCARD_TABLE_H
#define DISCARD_TABLE_H

#include "cards.h"
#include "discard.h"
#include "player.h"

float get_discard_table_mean(const Card cards[2], char is_my_crib);

Stats get_discard_table_stats(const Card cards[2], char is_my_crib, char is_flush);

void load_discard_tables(const char* filename); 

char min_max_stats(int cards_in_hand[5], Stats* stats, Stats* flush_stats);

void generate_min_max_discard_tables();

void generate_mean_discard_tables(int epochs, int num_hands, PlayerInfo* dealer, PlayerInfo* pone);

void print_discard_tables(); 

#endif
