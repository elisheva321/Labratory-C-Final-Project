/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  assembler.h
 * @brief  This file contains the declarations
 * of the functions and data structures used in the assembler.
 *********************************************************************/

#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "assembler_consts.h"
#include "../hash_table/hash_table.h"
#include "../assembler_helper/assembler_helper.h"


/**
 * Performs the first pass of the assembler.
 *
 * This function reads the input file and processes each line to build the symbol table and
 * generate the intermediate representation of the assembly code.
 *
 * @param assembler_data A pointer to the assembler data structure.
 * @param input_file     The input file to be processed.
 * @param macro_table    The macro table containing macro definitions.
 * @return               Returns 0 on success, or a non-zero value on failure.
 */
int assembler_first_pass(assembler_data_t *assembler_data, FILE *input_file, HashTable *macro_table);


/**
 * Performs the second pass of the assembly process.
 *
 * This function takes in the assembler data and the input file, and performs the second pass
 * of the assembly process. During the second pass, the function reads the input file line by line,
 * processes the instructions, and generates the corresponding machine code.
 *
 * @param assembler_data The data structure containing the assembler information.
 * @param input_file The input file to be processed.
 * @return Returns an integer indicating the success or failure of the second pass.
 *         A return value of 0 indicates success, while a non-zero value indicates failure.
 */
int assembler_second_pass(assembler_data_t *assembler_data, FILE *input_file);


/**
 * Initializes the assembler data structure.
 *
 * @return A pointer to the newly created assembler_data_t structure.
 */
assembler_data_t *init_assembler_data();


/**
 * Frees the memory allocated for the assembler data.
 *
 * @param assembler_data The pointer to the assembler data structure.
 * @return 0 if the memory is successfully freed, -1 otherwise.
 */
int free_assembler_data(assembler_data_t *assembler_data);


/**
 * Clears the assembler data.
 *
 * This function clears the given assembler data structure by resetting all its fields to their initial values.
 *
 * @param assembler_data The pointer to the assembler data structure to be cleared.
 */
void clear_assembler_data(assembler_data_t *assembler_data);


/**
 * Initializes the action into binary map.
 * This function performs the initialization of the action into binary map.
 *
 * @return Returns an integer value indicating the success or failure of the initialization.
 */
int init_action_into_binary_map();

#endif /* ASSEMBLER_H */
