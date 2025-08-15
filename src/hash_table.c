#include <stdlib.h>
#include <string.h>

#include "hash_table.h"

// Tombstone sentinel (special marker for deleted slots)
static ht_key_value_pair HT_DELETED_ITEM = {NULL, NULL};

// Load factor thresholds for resizing
#define HT_MAX_LOAD 0.7
#define HT_MIN_LOAD 0.15
#define HT_MIN_CAPACITY 8

// Create a new key-value pair
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

// Return a hash value for a given string (djb2)
static size_t ht_hash(const char* str) {
    size_t hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}

// Get the index for a key in the hash table
static size_t ht_get_index(const ht_hash_table* table, const char* key) {
    size_t hash = ht_hash(key);
    return hash % table->capacity;
}

// Resize the hash table to a new capacity
static int ht_resize(ht_hash_table* table, size_t new_capacity) {
    if (new_capacity < HT_MIN_CAPACITY) {
        new_capacity = HT_MIN_CAPACITY;
    }

    ht_key_value_pair** new_items = calloc(new_capacity, sizeof(ht_key_value_pair*));
    if (!new_items) return -1;

    // Rehash all valid items into the new array
    for (size_t i = 0; i < table->capacity; i++) {
        ht_key_value_pair* item = table->items[i];
        if (item && item != &HT_DELETED_ITEM) {
            size_t index = ht_hash(item->key) % new_capacity;
            for (size_t j = 0; j < new_capacity; j++) {
                size_t idx = (index + j * j) % new_capacity;
                if (new_items[idx] == NULL) {
                    new_items[idx] = item;
                    break;
                }
            }
        }
    }

    free(table->items);
    table->items = new_items;
    table->capacity = new_capacity;
    return 0;
}

// Create a new hash table
ht_hash_table* ht_new(size_t capacity) {
    if (capacity < HT_MIN_CAPACITY) {
        capacity = HT_MIN_CAPACITY;
    }

    ht_hash_table* table = malloc(sizeof(ht_hash_table));
    if (!table)
        return NULL;

    table->items = calloc(capacity, sizeof(ht_key_value_pair*)); // init items to NULL
    if (!table->items) {
        free(table);
        return NULL;
    }

    table->size = 0;
    table->capacity = capacity;

    return table;
}

// Delete a key-value pair
void ht_delete_item(ht_key_value_pair* item) {
    if (item) {
        free(item->value);
        free(item->key);
        free(item);
    }
}

// Delete a hash table
void ht_delete_hash_table(ht_hash_table* table) {
    if (table) {
        for (size_t i = 0; i < table->capacity; i++) {
            if (table->items[i] && table->items[i] != &HT_DELETED_ITEM) {
                ht_delete_item(table->items[i]);
            }
        }
        free(table->items);
        free(table);
    }
}

// Insert a key-value pair
void ht_insert(ht_hash_table* ht, const char* key, const char* value) {
    // Resize if load factor too high
    if ((double)(ht->size + 1) / ht->capacity > HT_MAX_LOAD) {
        ht_resize(ht, ht->capacity * 2);
    }

    size_t index = ht_get_index(ht, key);
    size_t first_tombstone = (size_t)-1;

    for (size_t i = 0; i < ht->capacity; i++) {
        size_t idx = (index + i * i) % ht->capacity;
        if (ht->items[idx] == NULL) {
            // Use tombstone if found earlier
            size_t target = (first_tombstone != (size_t)-1) ? first_tombstone : idx;
            ht_key_value_pair* new_item = ht_new_item(key, value);
            if (new_item) {
                ht->items[target] = new_item;
                ht->size++;
            }
            return;
        }
        if (ht->items[idx] == &HT_DELETED_ITEM) {
            if (first_tombstone == (size_t)-1) {
                first_tombstone = idx;
            }
            continue; // skip tombstones
        }
        if (strcmp(ht->items[idx]->key, key) == 0) {
            // Update value
            char* new_value = strdup(value);
            if (!new_value) return;
            free(ht->items[idx]->value);
            ht->items[idx]->value = new_value;
            return;
        }
    }
}

// Delete a key-value pair
void ht_delete(ht_hash_table* ht, const char* key) {
    size_t index = ht_get_index(ht, key);

    for (size_t i = 0; i < ht->capacity; i++) {
        size_t idx = (index + i * i) % ht->capacity;
        if (ht->items[idx] == NULL) {
            return; // Key not found
        }
        if (ht->items[idx] == &HT_DELETED_ITEM) {
            continue; // skip tombstones
        }
        if (strcmp(ht->items[idx]->key, key) == 0) {
            ht_delete_item(ht->items[idx]);
            ht->items[idx] = &HT_DELETED_ITEM;
            ht->size--;
            // Resize down if load factor too low
            if ((double)ht->size / ht->capacity < HT_MIN_LOAD) {
                ht_resize(ht, ht->capacity / 2);
            }
            return;
        }
    }
}

// Search for a value by key
char* ht_search(ht_hash_table* ht, const char* key) {
    size_t index = ht_get_index(ht, key);

    for (size_t i = 0; i < ht->capacity; i++) {
        size_t idx = (index + i * i) % ht->capacity;
        if (ht->items[idx] == NULL) {
            return NULL; // Key not found
        }
        if (ht->items[idx] == &HT_DELETED_ITEM) {
            continue; // skip tombstones
        }
        if (strcmp(ht->items[idx]->key, key) == 0) {
            return ht->items[idx]->value;
        }
    }

    return NULL;
}