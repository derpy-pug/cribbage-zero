#include "discard.h"
#include "discard_table.h"
#include "cards.h"
#include "cribbage_scoring.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void print_stats(const Stats stats) {
    printf("mean: %f, min: %d, max: %d", stats.mean, stats.min, stats.max);
}

void print_discard_stats(const DiscardStats discard_stats, const Hand hand) {
    printf("Discards: ");
    print_card(hand.cards[discard_stats.discards.first]);
    printf(" ");
    print_card(hand.cards[discard_stats.discards.second]);
    printf("\n");
    printf("Hand stats: ");
    print_stats(discard_stats.hand_stats);
    printf("\n");
    printf("Crib stats: ");
    print_stats(discard_stats.crib_stats);
    printf("\n");
    printf("Combined stats: ");
    print_stats(discard_stats.combined_stats);
    printf("\n");
}

void print_all_discard_stats(const DiscardStats* discard_stats, const Hand hand, int num_discard_stats) {
    for (int i = 0; i < num_discard_stats; i++) {
        printf("%d) ", i + 1);
        print_discard_stats(discard_stats[i], hand);
        printf("\n");
    }
}
void combined_stats(DiscardStats* stat, char is_my_crib) {
    if (is_my_crib) {
        stat->combined_stats.mean = stat->hand_stats.mean + stat->crib_stats.mean;
        stat->combined_stats.max = stat->hand_stats.max + stat->crib_stats.max;
        stat->combined_stats.min = stat->hand_stats.min + stat->crib_stats.min;
    } else {
        stat->combined_stats.mean = stat->hand_stats.mean - stat->crib_stats.mean;
        stat->combined_stats.max = stat->hand_stats.max - stat->crib_stats.min;
        stat->combined_stats.min = stat->hand_stats.min - stat->crib_stats.max;
    }
    stat->combined_stats.std_dev = 0;// TODO
} 

// TODO
Stats discard_stats_crib(const Card deck[46], const Hand Crib, char is_my_crib) {
    Stats stats = {0};
    stats.mean = get_discard_table_value(Crib.cards, is_my_crib);

    /*
    Card _crib[6];
    memcpy(_crib, Crib.cards, 2 * sizeof(Card));
    Hand crib = {_crib, 4};

    // TODO: optimize by removing suits
    stats.min = 29;
    for (int i = 0; i < 46; i++) {
        for (int j = i + 1; j < 46; j++) {
            for (int k = 0; k < 46; k++) {
                if (k == i || k == j) continue;
                crib.length -= 2;
                add_card_to_hand(&crib, deck[i]);
                add_card_to_hand(&crib, deck[j]);
                Card cut = deck[k];
                int score = score_hand(crib, cut, 1);
                if (score > stats.max) stats.max = score;
                if (score < stats.min) stats.min = score;
            }
        }
    }
    */
    // TODO: calculate standard deviation
    return stats;
}

Stats discard_stats_hand(const Card deck[46], const Hand hand) {
    Stats stats = {0};
    stats.min = 29;

    int sum = 0;
    for (int i = 0; i < 46; i++) {
        Card cut = deck[i];
        int score = score_hand(hand, cut, 0);
        sum += score;
        if (score > stats.max) stats.max = score;
        if (score < stats.min) stats.min = score;
    }
    stats.mean = sum / 46.0;
    // TODO: calculate standard deviation
    return stats;
}

DiscardStats discard_stats(const Hand hand, const Discard discards, char is_my_crib, const Card deck[46]) {

    // Create a new hand with the discard removed
    Card cards[4];
    Hand new_hand = {cards, 0};
    Card crib[4];
    Hand new_crib = {crib, 0};
    for (int i = 0; i < 6; i++) {
        if (i == discards.first || i == discards.second)
            add_card_to_hand(&new_crib, hand.cards[i]);
        else 
            add_card_to_hand(&new_hand, hand.cards[i]);
    }

    Stats hand_stats = discard_stats_hand(deck, new_hand);
    Stats crib_stats = discard_stats_crib(deck, new_crib, is_my_crib);

    Stats combined_stats_ = {0};
    DiscardStats discard_stats = {discards, hand_stats, crib_stats, combined_stats_};
    combined_stats(&discard_stats, is_my_crib);
    return discard_stats;
}

// Creates a length 15 size array of stats
// Must be freed by caller
DiscardStats* all_discard_stats(const Hand hand, char is_my_crib) {
    DiscardStats* stats = malloc(sizeof(DiscardStats) * 15); 

    Card deck[52];
    new_deck(deck);
    remove_cards_from_deck(deck, hand);

    int offset = -6;
    for (int i = 0; i < 6; i++) {
        offset += 5 - i;
        for (int j = i + 1; j < 6; j++) {
            int index = offset + j;
            Discard discards = {i, j};
            DiscardStats stat = discard_stats(hand, discards, is_my_crib, deck);
            stats[index] = stat;
        }
    }
    return stats;
}

DiscardStats best_discard(const Hand hand, char is_my_crib) {
    DiscardStats* stats = all_discard_stats(hand, is_my_crib);
    DiscardStats best = stats[0];
    for (int i = 1; i < 15; i++) {
        DiscardStats stat = stats[i];
        if (stat.combined_stats.mean > best.combined_stats.mean) best = stat;
    }
    free(stats);
    return best;
}

DiscardStats worst_discard(const Hand hand, char is_my_crib) {
    DiscardStats* stats = all_discard_stats(hand, is_my_crib);
    DiscardStats worst = stats[0];
    for (int i = 1; i < 15; i++) {
        DiscardStats stat = stats[i];
        if (stat.combined_stats.mean < worst.combined_stats.mean) worst = stat;
    }
    free(stats);
    return worst;
}

int compare_discard_stats_mean(const void* a, const void* b) {
    float mean_a = (*(DiscardStats*) a).combined_stats.mean;
    float mean_b = (*(DiscardStats*) b).combined_stats.mean;
    return (fabs(mean_a - mean_b) < 0.0001)? 0:
                              mean_b - mean_a > 0? 1: -1;
}

int compare_discard_stats_max(const void* a, const void* b) {
    DiscardStats stats_a = *(DiscardStats*) a;
    DiscardStats stats_b = *(DiscardStats*) b;
    return stats_b.combined_stats.max - stats_a.combined_stats.max;
}

// Creates a length 15 size array of stats
// Must be freed by caller
DiscardStats* sorted_discard_stats(const Hand hand, char is_my_crib) {
    DiscardStats* stats = all_discard_stats(hand, is_my_crib);
    qsort(stats, 15, sizeof(DiscardStats), compare_discard_stats_mean);
    return stats;
}
