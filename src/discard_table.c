#include "cards.h"
#include "cribbage.h"
#include "cribbage_scoring.h"
#include "discard.h"
#include "player.h"
#include "player_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static float table_my_crib[13][13] = {
    {5.2592,4.3760,4.7090,5.5536,5.8351,4.3427,4.3381,4.3792,4.0079,4.0660,4.3336,4.0517,4.1656},
    {0.0000,5.7671,7.2890,4.7786,5.8537,4.5036,4.4897,4.3479,4.4256,4.0343,4.3105,3.9939,4.0748},
    {0.0000,0.0000,5.9165,5.2001,6.6607,4.3157,4.3693,4.7482,4.3125,4.1618,4.3922,4.0653,4.2066},
    {0.0000,0.0000,0.0000,5.7006,6.8638,4.3944,4.3344,4.3846,4.0386,3.8120,4.1201,3.8119,4.0417},
    {0.0000,0.0000,0.0000,0.0000,9.1748,7.3408,6.4777,5.8760,5.8759,7.0118,7.2803,6.9307,6.8238},
    {0.0000,0.0000,0.0000,0.0000,0.0000,5.9858,5.3938,5.2755,5.7301,3.7009,3.9027,3.5925,3.6600},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,6.1913,7.1484,4.6837,3.7452,3.9763,3.7159,3.6210},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.7499,5.3354,4.3310,3.9659,3.7657,3.6626},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.3749,4.7267,4.4423,3.5117,3.4480},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.0223,4.8313,3.8189,3.2157},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.8430,5.0029,4.1900},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,4.9368,3.7370},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,4.8620}
}; 
static float table_pone_crib[13][13] = {
    {5.2782,4.4006,4.7314,5.5630,5.8700,4.3153,4.3668,4.3634,4.0120,4.0355,4.3438,4.0420,4.1877},
    {0.0000,5.6955,7.2867,4.8198,5.8999,4.5065,4.4584,4.3408,4.3987,4.0357,4.2938,3.9951,4.0653},
    {0.0000,0.0000,5.9513,5.1979,6.6672,4.3083,4.3720,4.7014,4.2902,4.1885,4.4221,4.1278,4.1713},
    {0.0000,0.0000,0.0000,5.7031,6.8497,4.3800,4.3553,4.3790,4.0412,3.8331,4.1845,3.8309,4.0646},
    {0.0000,0.0000,0.0000,0.0000,9.1612,7.3787,6.5330,5.8733,5.8811,6.9783,7.2997,7.0339,6.7607},
    {0.0000,0.0000,0.0000,0.0000,0.0000,5.9738,5.4114,5.2593,5.7067,3.6702,3.8718,3.5530,3.5795},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,6.1760,7.1521,4.6724,3.7081,3.9785,3.7085,3.6275},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.7705,5.3056,4.3447,3.9404,3.7198,3.6613},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.4039,4.7264,4.4244,3.4988,3.4593},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.0697,4.8107,3.8069,3.2144},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,5.7886,5.0137,4.2069},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,4.9414,3.7270},
    {0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,4.8370}
}; 


float get_discard_table_value(const Card cards[2], char is_my_crib) {
    int i = (int)cards[0].rank - 1;
    int j = (int)cards[1].rank - 1;
    if (i > j) {
        int temp = i;
        i = j;
        j = temp;
    }
    if (is_my_crib) {
        return table_my_crib[i][j];
    } else {
        return table_pone_crib[i][j];
    }
}

typedef struct {
    int sum;
    int freq;
} DiscardTableEntry;

DiscardTableEntry discard_my_crib[13][13];
DiscardTableEntry discard_pone_crib[13][13];

void init_discard_table() {
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            discard_my_crib[i][j].sum = 0;
            discard_my_crib[i][j].freq = 0;

            discard_pone_crib[i][j].sum = 0;
            discard_pone_crib[i][j].freq = 0;
        }
    }
}

void update_discard_table(Card cards[2], char is_my_crib, int score) {
    if (cards[0].rank > cards[1].rank) {
        Card temp = cards[0];
        cards[0] = cards[1];
        cards[1] = temp;
    }
    int i = (int)cards[0].rank - 1;
    int j = (int)cards[1].rank - 1;
    if (is_my_crib) {
        discard_my_crib[i][j].sum += score;
        discard_my_crib[i][j].freq++;
    } else {
        discard_pone_crib[i][j].sum += score;
        discard_pone_crib[i][j].freq++;
    }
}

void calculate_discard_table() {
    for (int i = 0; i < 13; i++) {
        for (int j = i; j < 13; j++) {
            if (discard_my_crib[i][j].freq > 0) {
                table_my_crib[i][j] = discard_my_crib[i][j].sum / (float)discard_my_crib[i][j].freq;
            } else {
                table_my_crib[i][j] = 0;
            }
            if (discard_pone_crib[i][j].freq > 0) {
                table_pone_crib[i][j] = discard_pone_crib[i][j].sum / (float)discard_pone_crib[i][j].freq;
            } else {
                table_pone_crib[i][j] = 0;
            }
        }
    }
}

void load_discard_tables(const char* filename) {
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening discard table file\n");
        return;
    }
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            fscanf(fp, "%f", &table_my_crib[i][j]);
        }
    }
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            fscanf(fp, "%f", &table_pone_crib[i][j]);
        }
    }
    fclose(fp);
}

void discard_table_to_file(const char* filename) {
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening discard table file\n");
        return;
    }
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            fprintf(fp, "%.*f ", 4, table_my_crib[i][j]);
        }
        fprintf(fp, "\n");
    }
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            fprintf(fp, "%.*f ", 4, table_pone_crib[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

void print_discard_tables() {
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            printf("%.*f ", 4, table_my_crib[i][j]);
        }
        printf("\n");
    }
     
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            printf("%.*f ", 4, table_pone_crib[i][j]);
        }
        printf("\n");
    }
}

void print_discard_tables_human_readable() {
    printf("Discard tables:\n");
    printf("\nMy crib:\n");
    // Header row
    printf("   ");
    for (int i = 1; i < 14; i++) {
        printf("%d ", i);
        if (i < 10) printf(" ");
    }
    printf("\n");
    // Print horizontal line
    printf("   ");
    for (int i = 0; i < 13; i++) {
        printf("---");
    }
    printf("\n");
    for (int i = 0; i < 13; i++) {
        printf("%d: ", i);
        for (int j = 0; j < 13; j++) {
            printf("%.*f ", 4, table_my_crib[i][j]);
        }
        printf("\n");
    }

    printf("\nPone crib:\n");
    // Header row
    printf("   ");
    for (int i = 1; i < 14; i++) {
        printf("%d ", i);
        if (i < 10) printf(" "); 
    }
    printf("\n");
    // Print horizontal line
    printf("   ");
    for (int i = 0; i < 13; i++) {
        printf("---");
    }
    // Print horizontal line
    printf("   ");
    for (int i = 0; i < 13; i++) {
        printf("---");
    }
    for (int i = 0; i < 13; i++) {
        printf("%d: ", i);
        for (int j = 0; j < 13; j++) {
            printf("%.*f ", 4, table_pone_crib[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_progress_bar(int epoch, int epochs) {
    int bar_width = 70;
    float progress = (float)epoch / (float)epochs;
    int pos = bar_width * progress;
    printf("[");
    for (int i = 0; i < bar_width; i++) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%\r", (int)(progress * 100.0));
}

void generate_discard_tables(int epochs, int num_hands, PlayerInfo* dealer, PlayerInfo* pone) {
    Card deck[52];
    new_deck(deck);
    shuffle_deck(deck);

    for (int i = 1; i <= epochs; i++) {
        init_discard_table();
        printf("Epoch %d\n", i);
        print_discard_tables();
        print_progress_bar(i, epochs);
        for (int j = 0; j < num_hands; j++) {
            char is_my_crib = j % 2 == 0;
            Card deckcpy[17];
            memcpy(deckcpy, deck, 13 * sizeof(Card));

            Hand dealer_hand = new_hand(deckcpy, 6);
            Hand pone_hand = new_hand(deckcpy + 6, 6);
            Hand crib = new_hand(deckcpy + 13, 0);
            Card cut = deckcpy[12];

            discard_turn(&dealer_hand, &crib, dealer, 1);
            discard_turn(&pone_hand, &crib, pone, 0);

            int score = score_hand(crib, cut, is_my_crib);
            Card* cards = crib.cards;
            update_discard_table(cards, is_my_crib, score);
            update_discard_table(cards + 2, !is_my_crib, score);

            shuffle_used_deck(deck, 17);
        }
        calculate_discard_table();
    }
    //print_progress_bar(epochs, epochs);
    //printf("\n");
    discard_table_to_file("txt/discard_table.txt");
}
