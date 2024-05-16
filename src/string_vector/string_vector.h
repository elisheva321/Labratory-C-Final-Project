/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file string_vector.h
 * @brief  This file contains the declarations of the string vector.
 * 
 *********************************************************************/

#ifndef STRING_VECTOR_H
#define STRING_VECTOR_H

/* Structure representing a vector of strings */
typedef struct {
    int size;
    int capacity;
    char **data;

} StringVector;

/* Function to create a new empty StringVector
   Complexity: O(1) */
StringVector* string_vector_create();

/* 
  Function to add a string to the end of the vector
  Parameters:
  vec: Pointer to the StringVector
  str: String to be added to the vector
  Complexity: O(1) amortized, O(n) worst-case (resizing) 
*/
int string_vector_push_back(StringVector* vec, const char* str);

/* Function to retrieve the string at a specified index in the vector
   Parameters:
   vec: Pointer to the StringVector
   index: Index of the element to retrieve
   Returns:
   Pointer to the string at the specified index
 Complexity: O(1) 
*/
const char* string_vector_at(StringVector* vec, size_t index);

/* Function to free the memory allocated for the StringVector
   Parameters:
   vec: Pointer to the StringVector
 	Complexity: O(n) 
*/
void string_vector_free(StringVector* vec);

/* return the size of the vector
   Parameters:
   vec: Pointer to the StringVector
 	Complexity: O(1) 
*/
int string_vector_get_size(StringVector* vec);

/* return the capacity of the vector
   Parameters:
   vec: Pointer to the StringVector
 	Complexity: O(n) 
*/
int string_vector_get_capacity(StringVector* vec);

/* print to the screen all content of the vector 
   Parameters:
   vec: Pointer to the StringVector
   Complexity: O(n) 
*/
void string_vector_print_vector(StringVector* vec);

/**
 * Clears all elements from the StringVector.
 *
 * This function removes all elements from the StringVector, making it empty.
 *
 * vec The StringVector to be cleared.
 */
void string_vector_clear(StringVector *vec);

#endif /* STRING_VECTOR_H */
