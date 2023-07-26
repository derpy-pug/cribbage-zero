#ifndef DISCARD_TABLE_H
#define DISCARD_TABLE_H

#include "cards.h"
#include "player.h"

float get_discard_table_value(const Card cards[2], char is_my_crib);

void load_discard_tables(const char* filename); 

void generate_discard_tables(int epochs, int num_hands, PlayerInfo* dealer, PlayerInfo* pone);

void print_discard_tables(); 

#endif
