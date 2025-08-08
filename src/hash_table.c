#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

// Create a new key-value pair
// Returns a pointer to the newly created item, or NULL on failure
static ht_key_value_pair* ht_new_item(const char* key, const char* value) {
    ht_key_value_pair* item = malloc(sizeof(ht_key_value_pair));
    if (!item)
        return NULL;

    item->key = strdup(key);
    if (!item->key) {
        free(item);
        return NULL;
    }

    item->value = strdup(value);
    if (!item->value) {
        free(item->key);
        free(item);
        return NULL;
    }

    return item;
}

// Create a new hash table with the specified capacity
// Returns a pointer to the newly created hash table, or NULL on failure
ht_hash_table* ht_new(size_t capacity) {
    ht_hash_table* table = malloc(sizeof(ht_hash_table));
    if (!table)
        return NULL;

    table->items = malloc(sizeof(ht_key_value_pair*) * capacity);
    if (!table->items) {
        free(table);
        return NULL;
    }

    for (size_t i = 0; i < capacity; i++) {
        table->items[i] = NULL;
    }

    table->size = 0;
    table->capacity = capacity;

    return table;
}

// Delete a key-value pair
// Frees the memory allocated for the key and value, and the item itself
void ht_delete_item(ht_key_value_pair* item) {
    if (item) {
        free(item->value);
        free(item->key);
        free(item);
    }
}

// Delete a hash table
// Frees all items in the table and the table itself
void ht_delete_hash_table(ht_hash_table* table) {
    if (table) {
        for (size_t i = 0; i < table->capacity; i++) {
            if (table->items[i] != NULL) { 
                ht_delete_item(table->items[i]);
            }
        }
        free(table->items);
        free(table);
    }
}

// Return a hash value for a given string
// Uses the djb2 algorithm for hashing
static size_t ht_hash(const char* str) {
    size_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

// Get the index for a key in the hash table
// Uses the hash function to compute the index
static size_t ht_get_index(const ht_hash_table* table, const char* key) {
    size_t hash = ht_hash(key);
    return hash % table->capacity;
}

// Insert a key-value pair into the hash table
// If the key already exists, update its value
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    size_t index = ht_get_index(ht, key);

    for (size_t i = 0; i < ht->capacity; i++) {
        size_t idx = (index + i * i) % ht->capacity; // quadratic probing
        if (ht->items[idx] == NULL) { 
            // Empty slot: insert new item
            ht_key_value_pair* new_item = ht_new_item(key, value);
            if (new_item) {
                ht->items[idx] = new_item;
                ht->size++;
            }
            return;
        }
        if (strcmp(ht->items[idx]->key, key) == 0) { 
            // Key exists: replace value
            ht_delete_item(ht->items[idx]);
            ht_key_value_pair* new_item = ht_new_item(key, value);
            if (new_item) {
                ht->items[idx] = new_item;
            }
            return;
        }
    }
}

// Delete a key-value pair from the hash table
// If the key is found, deletes it and decrements the size
void ht_delete(ht_hash_table* ht, const char* key) {
    size_t index = ht_get_index(ht, key);

    for (size_t i = 0; i < ht->capacity; i++) {
        size_t idx = (index + i * i) % ht->capacity; // quadratic probing
        if (ht->items[idx] == NULL) {
            return; // Key not found
        }
        if (strcmp(ht->items[idx]->key, key) == 0) {
            ht_delete_item(ht->items[idx]); 
            ht->items[idx] = NULL;
            ht->size--;
            return;
        }
    }
}

// Search for a value by key in the hash table
// Returns the value, or NULL if not found
char* ht_search(ht_hash_table* ht, const char* key) {
    size_t index = ht_get_index(ht, key);

    // Check if the key exists
    for (size_t i = 0; i < ht->capacity; i++) {
        size_t idx = (index + i * i) % ht->capacity; // quadratic probing
        if (ht->items[idx] == NULL) {
            return NULL; // Key not found
        }
        if (strcmp(ht->items[idx]->key, key) == 0) {
            return ht->items[idx]->value; 
        }
    }

    return NULL; // Key not found
}