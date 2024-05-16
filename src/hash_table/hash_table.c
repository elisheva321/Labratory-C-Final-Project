/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file hash_table.c
 * @brief  This file contains the implementation of the hash table data structure.
 * 
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../logger/logger.h"
#include "../general_const/general_const.h"
#include "../utils/utils.h"
#include "hash_table.h"

#define INITIAL_TABLE_SIZE 10
#define LOAD_FACTOR_THRESHOLD 0.75

/* Function to initialize the hash table */
HashTable *hash_table_init()
{
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    if (ht == NULL)
    {
        LOG(ERROR, PARAMS, "inside hash_table_init: Memory allocation for the hashtable failed!");
        return NULL;
    }
    ht->size = 0;
    ht->capacity = INITIAL_TABLE_SIZE;
    ht->buckets = (HashNode **)calloc(INITIAL_TABLE_SIZE, sizeof(HashNode *));
    if (ht->buckets == NULL)
    {
        LOG(ERROR, PARAMS, "inside hash_table_init: Memory allocation for the nodes failed!");
        free(ht);
        return NULL;
    }
    LOG(INFO, PARAMS, "inside hash_table_init: Hash table initialized successfully!");
    return ht;
}

/* Function to free the memory allocated for the hash table */
void hash_table_free(HashTable *ht, CustomFreeFunction free_function_data)
{
    int i = 0;
    for (i = 0; i < ht->capacity; i++)
    {
        HashNode *current = ht->buckets[i];
        while (current != NULL)
        {
            HashNode *temp = current;
            current = current->next;
            free(temp->key);
            free(temp);
        }
    }
    free(ht->buckets);
    free(ht);
}

/* Function to create a new hash node */
HashNode *hash_table_create_hash_node(const char *key, void *data)
{
    HashNode *new_node = (HashNode *)malloc(sizeof(HashNode));
    if (new_node == NULL)
    {
        LOG(ERROR, PARAMS, "inside create_hash_node: Memory allocation for the new node failed!");
        return NULL;
    }
    new_node->key = str_dup(key);
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

/* Function to compute the hash value for a given key */
unsigned int hash_table_hash(const char *key, size_t capacity)
{
    unsigned int hashValue = 0;
    while (*key)
    {
        hashValue = (hashValue << 5) + *key++;
    }
    return hashValue % capacity;
}

/* Function to insert a key-value pair into the hash table */
int hash_table_insert(HashTable *ht, const char *key, void *data)
{
    unsigned int index = hash_table_hash(key, ht->capacity);

    HashNode *new_node = hash_table_create_hash_node(key, data);
    if (new_node == NULL)
    { /* Check for node creation failure*/
        return ERR;
    }

    new_node->next = ht->buckets[index];
    ht->buckets[index] = new_node;
    ht->size++;

    /* Check if resizing is needed */
    if ((double)ht->size / ht->capacity >= LOAD_FACTOR_THRESHOLD)
    {
        if (hash_table_resize(ht) != SUCCESS)
        {
            return ERR;
        }
    }

    return SUCCESS; /* Indicate success*/
}
/* Function to resize the hash table */
int hash_table_resize(HashTable *ht)
{
    int i = 0;
    unsigned int new_index = 0;
    size_t new_capacity = ht->capacity * 2;
    HashNode **new_buckets = (HashNode **)calloc(new_capacity, sizeof(HashNode *));
    HashNode *next = NULL;
    if (new_buckets == NULL)
    {
        LOG(ERROR, PARAMS, "inside hash_table_resize: Memory allocation for the new node failed!");
        return ERR;
    }

    /* Rehash all existing elements */
    for (i = 0; i < ht->capacity; i++)
    {
        HashNode *current = ht->buckets[i];
        while (current != NULL)
        {
            next = current->next;
            new_index = hash_table_hash(current->key, new_capacity);
            current->next = new_buckets[new_index];
            new_buckets[new_index] = current;
            current = next;
        }
    }

    /* Update hash table properties */
    free(ht->buckets);
    ht->buckets = new_buckets;
    ht->capacity = new_capacity;
    return SUCCESS;
}

/* Function to retrieve the value associated with a key from the hash table */
void *hash_table_lookup(HashTable *ht, const char *key)
{
    unsigned int index = hash_table_hash(key, ht->capacity);
    HashNode *current = ht->buckets[index];
    while (current != NULL)
    {
        if (strcmp(current->key, key) == 0)
        {
            return current->data;
        }
        current = current->next;
    }
    return NULL; /* Key not found */
}

/* Function to traverse all elements in the hash table and call the callback function for each element */
void hash_table_for_each(HashTable *ht, HashTableForEachCallback callback, void *user_data)
{
	int i = 0;

    if (ht == NULL || callback == NULL)
    {
        LOG(ERROR, PARAMS, "inside hash_table_for_each: Hash table or callback is NULL!");
        return;
    }

    for (i = 0; i < ht->capacity; i++)
    {
        HashNode *current = ht->buckets[i];
        while (current != NULL)
        {
            callback(current->key, current->data, user_data);
            current = current->next;
        }
    }
}

/* Function to clear the hash table */
void hash_table_clear(HashTable *ht, CustomFreeFunction free_function_data)
{
    int i = 0;
    for (i = 0; i < ht->capacity; i++)
    {
        HashNode *current = ht->buckets[i];
        while (current != NULL)
        {
            HashNode *temp = current;
            current = current->next;
            free_function_data(temp->data); /* Free the data using the provided custom free function */
            free(temp->key);
            free(temp);
        }
        ht->buckets[i] = NULL; /* Set the bucket to NULL after clearing the linked list */
    }
    ht->size = 0; 
}
