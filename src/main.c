#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

static void print_table_state(ht_hash_table* table, const char* label) {
    printf("\n--- %s ---\n", label);
    printf("Capacity: %zu, Size: %zu\n", table->capacity, table->size);
    for (size_t i = 0; i < table->capacity; i++) {
        if (table->items[i] == NULL) {
            printf("[%zu] EMPTY\n", i);
        } else if (table->items[i] == (void*)&(ht_key_value_pair){NULL, NULL}) {
            printf("[%zu] TOMBSTONE\n", i);
        } else {
            printf("[%zu] %s => %s\n", i, table->items[i]->key, table->items[i]->value);
        }
    }
}

int main() {
    printf("Hash Table Test\n");

    ht_hash_table* table = ht_new(4); // small to trigger resizes quickly
    if (!table) {
        printf("Failed to create hash table.\n");
        return 1;
    }

    // Basic insert and search
    ht_insert(table, "apple", "red");
    ht_insert(table, "banana", "yellow");
    ht_insert(table, "grape", "purple");

    printf("Search apple: %s\n", ht_search(table, "apple"));
    printf("Search banana: %s\n", ht_search(table, "banana"));
    printf("Search grape: %s\n", ht_search(table, "grape"));

    // Update existing key
    ht_insert(table, "banana", "green");
    printf("Updated banana: %s\n", ht_search(table, "banana"));

    // Delete a key and check tombstone behavior
    ht_delete(table, "banana");
    printf("After deleting banana, search banana: %s\n", ht_search(table, "banana") ? "FOUND" : "NOT FOUND");

    // Reinsert banana to check tombstone reuse
    ht_insert(table, "banana", "yellow again");
    printf("Reinserted banana: %s\n", ht_search(table, "banana"));

    // Automatic resize up test
    for (int i = 0; i < 20; i++) {
        char key[32];
        char value[32];
        sprintf(key, "key%d", i);
        sprintf(value, "val%d", i);
        ht_insert(table, key, value);
    }
    print_table_state(table, "After bulk insert (resize up)");

    // Verify all keys exist
    int missing = 0;
    for (int i = 0; i < 20; i++) {
        char key[32];
        sprintf(key, "key%d", i);
        if (!ht_search(table, key)) {
            printf("Missing key: %s\n", key);
            missing++;
        }
    }
    printf("Missing keys after resize up: %d\n", missing);

    // Trigger automatic resize down
    for (int i = 0; i < 20; i++) {
        char key[32];
        sprintf(key, "key%d", i);
        ht_delete(table, key);
    }
    print_table_state(table, "After bulk delete (resize down)");

    // cleanup
    ht_delete_hash_table(table);
    printf("\nHash table deleted successfully.\n");

    return 0;
}