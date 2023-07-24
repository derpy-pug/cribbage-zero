#include "cribbage_scoring.h"
#include "hash_map.h"
#include <stdio.h>

static char primes[14] = { 0, 1, 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37 };

/*
 * Exact number of entries = 6175
 * Good prime just above = 6197
 * Good prime about half = 3089
 * Good prime about double = 12391
*/ 
static int score_table_size = 6197;

HashMap score_table;

int hash_score_prime(int key) {
   return (key - 2) % score_table_size;
}

//
/*
 * 1 -> 0
 * 2 -> 2
 * 3 -> 6
 * 4 -> 12
*/ 
static inline char score_exp_pair(char exp) {
	return (1 << exp) - 2 - (exp & 4); 	
}

struct card_set_type {
	char rank;
	char n;
};
char gen_card_set(struct card_set_type* set, const int ranks[5]) {
	int dist[14] = {0};
	for (int i = 0; i < 5; i++) {
		dist[ranks[i]]++;
	}
	int j = 0;
	for (int i = 1; i < 14; i++) {
			if (dist[i]) {
			struct card_set_type card_set_temp;
			card_set_temp.rank = i;
			card_set_temp.n = dist[i];
			set[j++] =  card_set_temp;
		}
	}
	return j;
}

char score_15(const int ranks[5]) {
	int score = 0;
	char interactions[32] = {0};
	int k = 0;
	for (int i = 0; i < 5; i++) {
        int rank = rank_value(ranks[i]);
		int k_temp = k;
		for (int j = 0; j < k_temp; j++) {
			int sum = interactions[j] + rank;
			if (sum == 15) score += 2;
			if (sum < 15) interactions[k++] = sum;
		}
		interactions[k++] = rank;
	}
	return score;
}

char score_hand_15s_runs_pairs(int hand[5]) {
    int score = 0;
	struct card_set_type set[5];
	char unique = gen_card_set(set, hand);
	
	char mult = 0;
	char length = 1;
	for(int i = 0; i < unique; i++) {
		if (set[i].n > 1) mult += set[i].n;
		if (i != unique - 1 && set[i+1].rank - set[i].rank == 1)
			length++;
		else { 
			if (length >= 3) break;
			mult = 0;
			length = 1;
		}
	}
	if (mult == 0) mult = 1;
	if (length >= 3) score += mult * length;

	for(int i = 0; i < unique; i++) {
		score += score_exp_pair(set[i].n);
	}

	score += score_15(hand);
    return score;
}

void init_scoring() {
    score_table = init_hash_map(score_table_size, hash_score_prime);
    int n = 0;
    for (int i1 = 1; i1 < 14; i1++) {
        for (int i2 = 1; i2 < 14; i2++) {
            for (int i3 = 1; i3 < 14; i3++) {
                for (int i4 = 1; i4 < 14; i4++) {
                    for (int i5 = 1; i5 < 14; i5++) {
                        if (i1 == i2 && i1 == i3 && i1 == i4 && i1 == i5)
                            continue;
                        int key = primes[i1] * primes[i2] * primes[i3] *
                            primes[i4] * primes[i5];
                        if (!hash_map_exists(score_table, key)) {
                            int ranks[5] = {i1, i2, i3, i4, i5};
                            int score = score_hand_15s_runs_pairs(ranks);
                            hash_map_put(score_table, key, score);
                            n++;
                        }
    }}}}} 
    /*
    print_hash_map(score_table);
    int size = hash_map_size(score_table);
    printf("score_table size: %d\n", size);
    printf("Scored %d hands\n", n);
    */
}

void check_scoring() {
    int n = 0;
    for (int i1 = 1; i1 < 14; i1++) {
        for (int i2 = 1; i2 < 14; i2++) {
            for (int i3 = 1; i3 < 14; i3++) {
                for (int i4 = 1; i4 < 14; i4++) {
                    for (int i5 = 1; i5 < 14; i5++) {
                        if (i1 == i2 && i1 == i3 && i1 == i4 && i1 == i5)
                            continue;
                        int key = primes[i1] * primes[i2] * primes[i3] *
                            primes[i4] * primes[i5];
                        if (!hash_map_exists(score_table, key)) {
                            printf("Missing key: %d\n", key);
                            printf("Missing hand: %d %d %d %d %d\n\n", i1, i2, i3, i4, i5);
                            n++;
                        }
    }}}}} 
    printf("Missing %d hands\n", n);
}

void free_scoring() {
    hash_map_del(score_table);
}

char score_flush(const Hand hand, Card cut, char is_crib) {
	char suit = hand.cards[0].suit;
	for (int i = 1; i < hand.length; i++) {
		if (hand.cards[i].suit != suit) return 0;
	}
	if (suit != cut.suit) {
		if (is_crib) return 0;
		else return 4;
	}
	return 5;
}

char score_knob(const Hand hand, Card cut) {
	for (int i = 0; i < hand.length; i++) {
		if (hand.cards[i].rank == 11 && hand.cards[i].suit == cut.suit) return 1;
	}
	return 0;
}

int get_hand_key(const Hand hand, Card cut) {
    int key = 1; 
    for (int i = 0; i < hand.length; i++) {
        key *= primes[(int)hand.cards[i].rank];
    }
    key *= primes[(int)cut.rank];
    return key;
}

// NOTE: add skip flush if it was impossible to get a flush
char score_hand(const Hand hand, Card cut, char is_crib) {
	char score = 0;
    score += hash_map_get(score_table, get_hand_key(hand, cut));
	score += score_flush(hand, cut, is_crib);
	score += score_knob(hand, cut);
	return score;
}

/*
char score_run(Card* hand_cut_sorted) {
	char mult = 1;
	char length = 1;
	for (int i = 1; i < 5; i++) {
		char right = hand_cut_sorted[i].rank;
		char left = hand_cut_sorted[i-1].rank;
		char delta = right - left;

		if (delta == 0) {
			if (i > 1 && right == hand_cut_sorted[i - 2].rank) mult = 3;
			else mult = mult << 1;	
		}
		
		else if (delta == 1) {
			length++;
		}

		else if (delta > 1) {
			if (length >= 3) break;
			mult = 1;
			length = 1;
		}
		
	}
	if (length >= 3) return length * mult;
	return 0;
}
char score_pair(Card* hand_cut_sorted) {
	char score = 0;
	char exp = 1;
	
	for (int i = 1; i < 5; i++) {
		char right = hand_cut_sorted[i].rank;
		char left = hand_cut_sorted[i-1].rank;
	
		if (right == left) exp++;
		else {
			score += score_exp_pair(exp); 
			exp = 1;
		}			
	}
	return score + score_exp_pair(exp);
}
*/

char score_pile_run(const CardPile pile) {
    char min;
    char max;
    for (int i = 0; i < pile.length - 2; i++) {
        min = pile.cards[i].rank;
        max = pile.cards[i].rank;
        for (int j = i + 1; j < pile.length; j++) {
            char rank = pile.cards[j].rank;
            if (rank < min) min = rank;
            if (rank > max) max = rank;
        }
        if (max - min == pile.length - i - 1) {
            return max-min;
        }
    }
    return 0;
}

char score_pile(const CardPile pile) {
    char score = 0;
    // Score based off the card and the pile
    char sum = pile.sum_31;
    // Score 31
    if (sum == 31) {
        score += 1;
    }
    // Score 15
    if (sum == 15) {
        score += 2;
    }
    // Score pairs
    int pairs = 1;
    for (int i = pile.length - 1; i > 0; i--) {
        if (pile.cards[i].rank == pile.cards[i - 1].rank) {
            pairs++;
        } else {
            score += score_exp_pair(pairs); 
            break;
        }
    }
    // Score runs
    score += score_pile_run(pile);
    return score;
}
