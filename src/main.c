#include <stdio.h>
#include <stdlib.h>

#include "hash_table.h"


int main() { 
    printf("Hash Table Example\n");
    // Example usage of the hash table
    ht_hash_table* table = ht_new(10);
    if (!table) {
        printf("Failed to create hash table.\n");
        return 1; // Failed to create hash table
    }

    ht_insert(table, "key1", "value1");
    ht_insert(table, "key2", "value2");

    char* value = ht_search(table, "key1");
    if (value) {
        printf("Found key1: %s\n", value);
    } else {
        printf("key1 not found\n");
    }

    ht_delete(table, "key1");
    value = ht_search(table, "key1");
    if (value) {
        printf("Found key1: %s\n", value);
    } else {
        printf("key1 not found after deletion\n");
    }

    // Clean up
    ht_delete_hash_table(table);
    // Note: ht_delete_hash_table should free all items and the table itself
    printf("Hash table deleted successfully.\n");
    return 0;
}