/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file preprocessor.h
 * @brief This file contains the declarations for the preprocessor module.
 * 
 *********************************************************************/


#ifndef PRE_PROCESSOR_H
#define PRE_PROCESSOR_H
#include "../general_const/general_const.h"
#include "../hash_table/hash_table.h"
#include "../string_vector/string_vector.h"


/* This structure is used for general data pre-processing operations.
 * It can be used to store and manipulate data before further processing. */
typedef struct general_data_pre_process
{
	StringVector *error_founds; /* A vector to store errors found during pre-processing */
	StringVector *warning_founds; /* A vector to store warnings found during pre-processing */
	HashTable *macro_table; /* A hash table to store macros. key: macro name. value: macro content*/

} general_data_pre_process_t;


/**
 * Initializes the pre-processing of general data.
 *
 * This function creates and returns a pointer to a `general_data_pre_process_t` structure,
 * which is used for pre-processing general data.
 *
 * @return A pointer to the initialized `general_data_pre_process_t` structure.
 */
general_data_pre_process_t *init_pre_process_general_data();


/**
 * Pre-processes the input data from the given input file.
 *
 * @param pre_process_data A pointer to the general_data_pre_process_t struct that holds pre-processing data.
 * @param input_file The input file to read the data from.
 * @param file_name The name of the input file.
 * @param output_file_name The name of the output file to write the pre-processed data to.
 * @return Returns an integer value indicating the success or failure of the pre-processing operation.
 */
int pre_process_input(general_data_pre_process_t *pre_process_data, FILE *input_file, const char *file_name, char *output_file_name);

/**
 * @brief Clears the pre-process data structure.
 *
 * This function clears the pre-process data structure by resetting all its fields to their default values.
 *
 * @param pre_process_data A pointer to the pre-process data structure to be cleared.
 */
void clear_pre_process_data(general_data_pre_process_t *pre_process_data);


/**
 * Frees the memory allocated for the general_data_pre_process_t structure.
 *
 * @param general_data_pre_process A pointer to the general_data_pre_process_t structure to be freed.
 */
void free_pre_process_data(general_data_pre_process_t *general_data_pre_process);


/**
 * @struct macro_data
 * @brief Represents the data structure for storing macro information.
 */
typedef struct macro_data {
	char macro_name[MAX_LABEL_SIZE]; /* The name of the macro */
	StringVector *macro_content; /* The content of the macro */
} macro_data_t;

#endif /* PRE_PROCESSOR_H */
