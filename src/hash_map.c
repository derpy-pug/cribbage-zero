#include "hash_map.h"
#include <stdlib.h>
#include <stdio.h>


HashMap init_hash_map(int buckets, int (*hash_fn)(int key)) {
    HashMap map;
    map.table_array = malloc(buckets * sizeof(HashNode));
    map.hash_fn = hash_fn;
    map.buckets = buckets;

    HashNode empty_node =  { 0, -1, 0 };
    for (int i  = 0; i < map.buckets; i++) {
        map.table_array[i] = empty_node; 
    }
    return map;
}

void hash_map_put(HashMap map, int key, int value) {
    int index = map.hash_fn(key);
    HashNode* node = &map.table_array[index];
    while(1) {
        if (node->key < 0) {
            node->next = 0;
            node->key = key;
            node->value = value;
            break;
        }
        else if (node->key >= 0) {
            if (node->key == key) {
                node->value = value;
                break;
            }
            if (node->next) {
                node = node->next;
                continue;
            }
            HashNode* new_node = malloc(sizeof(HashNode));
            new_node->next = 0;
            new_node->key = -1;
            new_node->value = 0;
            node->next = new_node;
            node = node->next;
        }
    }
}

int hash_map_get(const HashMap map, int key) {
    int index = map.hash_fn(key);
    HashNode* node = &map.table_array[index];
    while(1) {
        if (node->key == key) {
            return node->value;
        }
        if (!node->next) return -1;
        node = node->next;
    }
}

char hash_map_exists(const HashMap map, int key) {
    int index = map.hash_fn(key);
    HashNode* node = &map.table_array[index];
    while(1) {
        if (node->key == key) {
            return 1;
        }
        if (!node->next) return 0;
        node = node->next;
    }
}

void free_list(HashNode* node) {
    HashNode* head = node;
    while(1) {
        node = head;
        if (!head->next) break;
        HashNode* prev = head;
        node = node->next;
        while(1) {
           if (!node->next) {
                free(node);
                prev->next = 0;
                break;
           }
           prev = node;
           node = node->next;
        }
    }
    free(head);
}

void hash_map_del(HashMap map) {
    for (int i = 0; i < map.buckets; i++) {
        HashNode* node = &map.table_array[i];
        if (node->next) free_list(node->next);
    }
    free(map.table_array);
    map.table_array = 0;
}

void print_hash_map(const HashMap map) {
    for (int i = 0; i < map.buckets; i++) {
        HashNode* node = &map.table_array[i];
        printf("%d: ", i);
        while(1) {
            if (node->key < 0) {
                printf("NULL\n");
                break;
            }
            printf("(%d, %d) ", node->key, node->value);
            if (!node->next) {
                printf("\n");
                break;
            }
            node = node->next;
        }
    }
}

int hash_map_size(const HashMap map) {
    int size = 0;
    for (int i = 0; i < map.buckets; i++) {
        HashNode* node = &map.table_array[i];
        while(1) {
            if (node->key < 0) break;
            size++;
            if (!node->next) break;
            node = node->next;
        }
    }
    return size;
}

