/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file string_vector.c    
 * @brief This file contains the implementation of a string vector.
 * 
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_vector.h"
#include "../logger/logger.h"
#include "../general_const/general_const.h"
#include "../utils/utils.h"

#define INITIAL_CAPACITY 10

StringVector *string_vector_create()
{
    StringVector *vec = (StringVector *)malloc(sizeof(StringVector));
    if (vec == NULL)
    {
        LOG(ERROR, PARAMS, "Memory allocation failed in string_vector_create for vec");
        return NULL;
    }
    vec->data = malloc(INITIAL_CAPACITY * sizeof(char *));
    memset(vec->data, 0, INITIAL_CAPACITY * sizeof(char *));
    if (vec->data == NULL)
    {
        LOG(ERROR, PARAMS, "Memory allocation failed in string_vector_create for vec->data");
        free(vec);
        return NULL;
    }
    vec->size = 0;
    vec->capacity = INITIAL_CAPACITY;
    return vec;
}

int string_vector_push_back(StringVector *vec, const char *str)
{
    size_t new_capacity;
    char *new_str = NULL;
    char **new_data = NULL;

    if (vec->size == vec->capacity)
    {
        /* Need to reallocate memory */
        new_capacity = vec->capacity * 2;
        new_data = realloc(vec->data, new_capacity * sizeof(char *));
        if (new_data == NULL)
        {
            LOG(ERROR, PARAMS, "Memory reallocation failed in string_vector_push_back");
            return ERR;
        }
        vec->data = new_data;
        vec->capacity = new_capacity;
    }

    /* Copy the string and add it to the vector */
    new_str = str_dup(str);
    if (new_str == NULL)
    {
        LOG(ERROR, PARAMS, "Memory allocation failed in string_vector_push_back for new_str");
        return ERR;
    }
    vec->data[vec->size] = new_str;
    vec->size++;
    return SUCCESS;
}
const char *string_vector_at(StringVector *vec, size_t index)
{
    if (index >= vec->size)
    {
        LOG(ERROR, PARAMS, "string_vector_at Index out of bounds! returning error...");
        return NULL;
    }
    return vec->data[index];
}

void string_vector_free(StringVector *vec)
{
    int i = 0;
    for (i = 0; i < vec->size; i++)
    {
        if (vec->data[i] != NULL)
        {
            free(vec->data[i]);
        }
    }
    free(vec->data);
    free(vec);
}

int string_vector_get_size(StringVector *vec)
{
    return vec->size;
}

int string_vector_get_capacity(StringVector *vec)
{
    return vec->capacity;
}

void string_vector_print_vector(StringVector *vec)
{

    int i = 0;

    for (i = 0; i < vec->size; ++i)
    {
        printf("\nvec[%d]= %s\n", i, string_vector_at(vec, i));
    }
}
void string_vector_clear(StringVector *vec)
{
	int i ;
    for (i = 0; i < vec->size; i++)
    {
        free(vec->data[i]);
        vec->data[i] = NULL;
    }
    vec->size = 0;
}
