/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  assembler_main.h
 * @brief This file contains the implementation of the main assembler functions.
 * 
 *********************************************************************/

#ifndef ASSEMBLER_MAIN_H
#define ASSEMBLER_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../general_const/general_const.h"
#include "../utils/utils.h"
#include "../hash_table/hash_table.h"
#include "../string_vector/string_vector.h"

/**
 * @brief Assembles the input files.
 *
 * This function takes an array of input file names and the number of input files as parameters.
 * It assembles the input files and returns an integer value indicating the status of the assembly process.
 *
 * @param input_file_names An array of input file names.
 * @param num_input_files The number of input files.
 * @return An integer value indicating the status of the assembly process.
 */
int assembler_inputs(const char *input_file_names[], int num_input_files);

#endif /* ASSEMBLER_MAIN_H */
