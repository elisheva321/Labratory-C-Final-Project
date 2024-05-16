/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file hash_table.h
 * @brief  This file contains function declarations of the hash table data structure.
 * 
 *********************************************************************/
#ifndef HASH_TABLE_H
#define HASH_TABLE_H

/* Define a structure for a hash node */
typedef struct HashNode
{
    char *key;
    void *data;
    struct HashNode *next;
} HashNode;

/* Define a structure for the hash table */
typedef struct
{
    size_t size;        /* Current size of the hash table */
    size_t capacity;    /* Capacity of the hash table */
    HashNode **buckets; /* Array of buckets */
} HashTable;

/* Function prototype for the callback function  for free unction*/
typedef void (*CustomFreeFunction)(void *);

/* Function prototype for the callback function  for for each function*/
typedef void (*HashTableForEachCallback)(const char *key, void *data, void *user_data);

/*
 * Function to initialize a hash table with a default capacity.
 *
 * Returns:
 *   Pointer to the initialized hash table
 */
HashTable *hash_table_init();

/*
 * Function to free the memory allocated for the hash table and its elements.
 *
 * Parameters:
 *   ht: Pointer to the hash table
 *   free_function_data pointer to a function to free data.
 */
void hash_table_free(HashTable *ht, CustomFreeFunction free_function_data);

/*
 * Function to compute the hash value for a given key.
 *
 * Parameters:
 *   key: The key for which the hash value needs to be computed
 *   capacity: The capacity of the hash table (number of buckets)
 *
 * Returns:
 *   The computed hash value
 */
unsigned int hash_table_hash(const char *key, size_t capacity);

/*
 * Function to insert a key-value pair into the hash table.
 *
 * Parameters:
 *   ht: Pointer to the hash table
 *   key: The key associated with the data
 *   data: Pointer to the data
 */
int hash_table_insert(HashTable *ht, const char *key, void *data);

/*
 * Function to resize the hash table when the load factor exceeds a threshold.
 *
 * Parameters:
 *   ht: Pointer to the hash table
 */
int hash_table_resize(HashTable *ht);

/*
 * Function to retrieve the data associated with a given key from the hash table.
 *
 * Parameters:
 *   ht: Pointer to the hash table
 *   key: The key for which the data needs to be retrieved
 *
 * Returns:
 *   Pointer to the data associated with the key, or NULL if the key is not found
 */
void *hash_table_lookup(HashTable *ht, const char *key);

/*
 * Function to create a new hash node.
 *
 * Parameters:
 *   key: The key associated with the data
 *   data: Pointer to the data
 *
 * Returns:
 *   Pointer to the newly created hash node
 */
HashNode *hash_table_create_hash_node(const char *key, void *data);

/*
 * Function to clear the hash from all its content.
 *
 * Parameters:
 *   ht: Pointer to the hash table
 *   free_function_data: pointer to a function to free data
 */
void hash_table_clear(HashTable *ht, CustomFreeFunction free_function_data);

/*
 * Function to perform a given callback from the user for each element in the hash.
 *
 * Parameters:
 *   ht: Pointer to the hash table
 *   free_function_data: pointer to a function to free data
 */
void hash_table_for_each(HashTable *ht, HashTableForEachCallback callback, void *data);



#endif /* HASH_TABLE_H */
