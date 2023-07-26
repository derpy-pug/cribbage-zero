#ifndef DISCARD_H
#define DISCARD_H

#include "cards.h"

typedef struct {
    float mean;
    float std_dev;
    int min;
    int max;
} Stats;

typedef struct {
    int first, second;
} Discard;

typedef struct {
    Discard discards;
    Stats hand_stats;
    Stats crib_stats;
    Stats combined_stats;
} DiscardStats;

DiscardStats best_discard(const Hand hand, char is_my_crib);

DiscardStats worst_discard(const Hand hand, char is_my_crib);

DiscardStats* sorted_discard_stats(const Hand hand, char is_my_crib);

void print_discard_stats(const DiscardStats discard_stats, const Hand hand);

void print_all_discard_stats(const DiscardStats* discard_stats, const Hand hand, int num_discard_stats);

#endif
