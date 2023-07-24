#ifndef HASH_MAP
#define HASH_MAP

#include <stdlib.h>

/*
 * Requires key is non-negative
 * If key is negative the node does not exist
*/
typedef struct hash_node {
    struct hash_node* next; 
    int key;
    int value;
} HashNode;

typedef struct {
    HashNode* table_array;
    int (*hash_fn)(int key);
    int buckets;
} HashMap;

HashMap init_hash_map(int buckets, int (*hash_fn)(int key));

void hash_map_put(HashMap map, int key, int value);

int hash_map_get(const HashMap map, int key);

char hash_map_exists(const HashMap map, int key);

void hash_map_del(HashMap map);

void print_hash_map(const HashMap map);

int hash_map_size(const HashMap map);

#endif
