#ifndef HT_HASH_TABLE_H
#define HT_HASH_TABLE_H

#include <stddef.h>



/* TYPES */


typedef struct {
    char* key;
    char* value;
} ht_key_value_pair;

typedef struct {
    ht_key_value_pair** items;
    size_t size;
    size_t capacity;
} ht_hash_table;



/* API FUNCTIONS */



// Creates a new hash table with the specified capacity
// Returns a pointer to the newly created hash table, or NULL on failure
ht_hash_table* ht_new(size_t capacity);

// Deletes a key-value pair
// Frees the memory allocated for the key and value, and the item itself
void ht_delete_item(ht_key_value_pair* item);

// Deletes a hash table
// Frees all items in the table and the table itself
void ht_delete_hash_table(ht_hash_table* table);

// Inserts a key-value pair into the hash table
// If the key already exists, updates its value
void ht_insert(ht_hash_table* ht, const char* key, const char* value);

// Deletes a key-value pair from the hash table
// If the key is found, deletes it and decrements the size
void ht_delete(ht_hash_table* ht, const char* key);

// Searches for a value by key in the hash table
// Returns the value, or NULL if not found
char* ht_search(ht_hash_table* ht, const char* key);



#endif /* HT_HASH_TABLE_H*/