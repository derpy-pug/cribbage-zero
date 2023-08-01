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

// TODO
float discard_stats_crib_mean(const Hand crib, char is_my_crib) {
    char is_flush = crib.cards[0].suit == crib.cards[1].suit;
    Stats stats = get_discard_table_stats(crib.cards, is_my_crib, is_flush);
    if (is_flush) {
        // Expected value of a flush given 2 cards of the same suit
        // and how many cards of that suit are in their remaining hand
        // This assumes that the oppenents discards suits are random
        // 0 in hand: 0.108696 * 5 = 0.054348
        // 1 in hand: 0.007905 * 5 = 0.039526
        // 2 in hand: 0.005533 * 5 = 0.027668
        // 3 in hand: 0.003689 * 5 = 0.018445
        // 4 in hand: 0.002306 * 5 = 0.011528
        stats.mean += 0.054348;
    }
    return stats.mean;

    /*
    Card _crib[4];
    memcpy(_crib, crib.cards, 2 * sizeof(Card));
    Hand crib = {_crib, 4};

    // TODO: optimize by removing suits
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

void update_min_max_stats(Stats* crib_stats, Stats* crib_stats_temp, Stats* hand_stats, int hand_score, Stats* combined_stats, char is_my_crib) {
        if (crib_stats_temp->max > crib_stats->max) crib_stats->max = crib_stats_temp->max;
        if (crib_stats_temp->min < crib_stats->min) crib_stats->min = crib_stats_temp->min;

        if (hand_score > hand_stats->max) hand_stats->max = hand_score;
        if (hand_score < hand_stats->min) hand_stats->min = hand_score;

        if (is_my_crib) {
            crib_stats_temp->min += hand_score;
            crib_stats_temp->max += hand_score;
        }
        else {
            int min_temp = crib_stats_temp->min;
            crib_stats_temp->min = hand_score - crib_stats_temp->max;
            crib_stats_temp->max = hand_score - min_temp;
        }
        if (crib_stats_temp->max > combined_stats->max) {
            combined_stats->max = crib_stats_temp->max;
        }
        if (crib_stats_temp->min < combined_stats->min) {
            combined_stats->min = crib_stats_temp->min;
        }
}

/*
 * Requires the deck is sorted by rank
*/ 
DiscardStats discard_stats_combined(const Hand hand, const Discard discards, char is_my_crib, const Card deck[46]) {
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
    
    Stats hand_stats = {0};
    Stats crib_stats = {0};
    Stats combined_stats = {0};
    hand_stats.min = 42;
    crib_stats.min = 42;
    combined_stats.min = 42;

    int cards_count[13] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
    for (int i = 0; i < 6; i++) {
        cards_count[(int)hand.cards[i].rank]--;
    }

    int cards_in_hand[5];
    cards_in_hand[1] = (int)new_crib.cards[0].rank;
    cards_in_hand[2] = (int)new_crib.cards[1].rank;

    int hand_sum = 0;
    for (int i = 0; i < 46; i++) {
        Card cut = deck[i];
        int cut_rank = (int)cut.rank;
        cards_count[cut_rank]--;
        int hand_score = score_hand(new_hand, cut, 0);
        //char hand_flush_score = score_flush(hand, cut, 0);
        hand_sum += hand_score;

        cards_in_hand[0] = cut_rank;

        char init_is_possible_knob = new_crib.cards[0].rank == 11 && new_crib.cards[0].suit == cut.suit;
        init_is_possible_knob |= new_crib.cards[1].rank == 11 && new_crib.cards[1].suit == cut.suit;
        char init_is_possible_flush = new_crib.cards[0].suit == cut.suit && new_crib.cards[1].suit == cut.suit;
        Stats crib_stats_temp = {0};
        crib_stats_temp.min = 42;
        for (int j = 1; j < 14; j++) {
            if (cards_count[j] == 0) continue;
            cards_in_hand[3] = j;
            cards_count[j]--;
            for (int k = j; k < 14; k++) {
                if (cards_count[k] == 0) continue;
                cards_in_hand[4] = k;
                cards_count[k]--;

                // j and k need to be different ranks and different to the cut rank
                // j and k cards need to not be in the hand
                char is_flush = 0;
                if (init_is_possible_flush) {
                    if (j != cut_rank && k != cut_rank && j != k) {
                        is_flush = 1;
                        for (int card_index = 0; card_index < hand.length; card_index++) {
                            Card card = hand.cards[card_index];
                            if (j == card.rank && card.suit == cut.suit) {
                                is_flush = 0;
                                break;
                            }
                            if (k == card.rank && card.suit == cut.suit) {
                                is_flush = 0;
                                break;
                            }
                        }
                    }
                }

                char is_knob = 0;
                if (init_is_possible_knob) {
                    is_knob = 1;
                }
                else if (cut_rank != 11 && (j == 11 || k == 11)){
                    is_knob = 1;
                    for (int card_index = 0; card_index < hand.length; card_index++) {
                        Card card = hand.cards[card_index];
                        if (card.rank == 11 && card.suit == cut.suit) {
                            is_knob = 0;
                            break;
                        }
                    }
                }


                int crib_score = score_hand_suitless(cards_in_hand);
                crib_score += is_flush ? 5 : 0;
                crib_score += is_knob ? 1 : 0;
                if (crib_score > crib_stats_temp.max) crib_stats_temp.max = crib_score;
                if (crib_score < crib_stats_temp.min) crib_stats_temp.min = crib_score;

                cards_count[k]++;
            }
            cards_count[j]++;
        }

        update_min_max_stats(&crib_stats, &crib_stats_temp, 
                             &hand_stats, hand_score, 
                             &combined_stats, is_my_crib);

        cards_count[cut_rank]++;
    }
    hand_stats.mean = hand_sum / 46.0;
    crib_stats.mean = discard_stats_crib_mean(new_crib, is_my_crib);

    // Not completely accurate, but close enough for now
    // They are not independent, so can't just add them
    // TODO: Fix this
    if (is_my_crib) {
        combined_stats.mean = hand_stats.mean + crib_stats.mean ;
    }
    else {
        combined_stats.mean = hand_stats.mean - crib_stats.mean;
    }

    DiscardStats discard_stats = {discards, hand_stats, crib_stats, combined_stats};
    return discard_stats;
}

// Does not compute combined_stats or min/max stats for crib
// FAST
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
    Stats crib_stats = {0};
    crib_stats.mean = discard_stats_crib_mean(new_crib, is_my_crib);

    Stats combined_stats_ = {0};
    if (is_my_crib) {
        combined_stats_.mean = hand_stats.mean + crib_stats.mean ;
    }
    else {
        combined_stats_.mean = hand_stats.mean - crib_stats.mean;
    }
    DiscardStats discard_stats = {discards, hand_stats, crib_stats, combined_stats_};
    return discard_stats;
}

// Creates a length 15 size array of stats
// Must be freed by caller
DiscardStats* all_discard_stats(const Hand hand, char is_my_crib) {
    DiscardStats* stats = malloc(sizeof(DiscardStats) * 15); 

    Card deck[52];
    init_deck(deck);
    remove_cards_from_deck_keep_order(deck, hand);

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
