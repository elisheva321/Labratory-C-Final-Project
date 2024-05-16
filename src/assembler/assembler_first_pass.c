/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file assembler_first_pass.c
 * @brief This file contains the implementation 
 * of the first pass of the assembler.
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../utils/utils.h"
#include "assembler.h"

#include "../logger/logger.h"
#include "../general_const/general_const.h"

#include "../hash_table/hash_table.h"
#include "../string_vector/string_vector.h"

/* =========================== internal functions declarations ================== */
void _free_data(void *ptr);
void _do_nothing(void *prt);


/* =========================== internal functions implementations ================= */

void _free_data(void *ptr)
{
	if (NULL != ptr)
	{
		free(ptr);
	}
}
void _do_nothing(void *ptr)
{
	UNUSED(ptr);
	return;
}

/*=============================================================================== */

assembler_data_t *init_assembler_data(int should_allocate_memory)
{
	assembler_data_t *assembler_data = NULL;

	LOG(INFO, PARAMS, "initializing assembler data \n");

	assembler_data = (assembler_data_t *)malloc(sizeof(assembler_data_t));
	memset(assembler_data, 0, sizeof(assembler_data_t));
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(assembler_data, "Memory allocation for the assembler_data failed", NULL);

	assembler_data->IC = 100;
	assembler_data->DC = 0;

	init_line_handlers_arr(assembler_data);
	assembler_data->symbol_table = hash_table_init();
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(assembler_data->symbol_table, "Initialization of symbol_table failed", NULL);

	/*initializing action_into_binary_map */
	if (init_action_into_binary_map(assembler_data) != SUCCESS)
	{
		LOG(ERROR, PARAMS, "inside init_assembler_data: Initialization of action_into_binary_map failed");
		return NULL;
	}

	/*initializing warning_founds */
	assembler_data->warning_founds = string_vector_create();
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(assembler_data->warning_founds, "Initialization of warning_founds failed", NULL);

	/*initializing error_founds */
	assembler_data->error_founds = string_vector_create();
	if (assembler_data->error_founds == NULL)
	{
		LOG(ERROR, PARAMS, "inside init_assembler_data: Initialization of error_founds failed");
		return NULL;
	}

	/*initializing parsed_instruction_lines */
	assembler_data->parsed_instruction_lines = queue_create();
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(assembler_data->parsed_instruction_lines, "Initialization of parsed_instruction_lines failed", NULL);

	/*initializing extern_symbols */
	assembler_data->extern_symbols = queue_create();
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(assembler_data->extern_symbols, "Initialization of extern_symbols failed", NULL);

	/*initializing line_count */
	assembler_data->line_count = 1;
	assembler_data->has_entry = FALSE;

	return assembler_data;
}

int init_action_into_binary_map(assembler_data_t *assembler_data)
{
	int returned_val = 0;
	int i = 0;

	assembler_data->action_into_binary_map = hash_table_init();
	if (assembler_data->action_into_binary_map == NULL)
	{
		return ERR;
	}
	assembler_data->instruction_code_arr = (int *)malloc(NUM_INSTRUCTIONS * sizeof(int));
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(assembler_data->instruction_code_arr, "Memory allocation for array failed at init_action_into_binary_map", ERR);

	for (i = 0; i < NUM_INSTRUCTIONS; i++)
	{
		assembler_data->instruction_code_arr[i] = i;
		/* inserting the instructions into the hash table */
		returned_val += hash_table_insert(assembler_data->action_into_binary_map, INSTRUCTIONS_NAMES[i], (void *)(assembler_data->instruction_code_arr + i));
	}

	if (returned_val != 0)
	{
		LOG(ERROR, PARAMS, "hash_table_insert function failed at init_action_into_binary_map\n");
		return ERR;
	}

	return SUCCESS;
}

int free_assembler_data(assembler_data_t *assembler_data)
{
	/*in case assembler_data is null*/
	if (assembler_data == NULL)
	{
		return ERR;
	}

	free(assembler_data->instruction_code_arr);
	hash_table_free(assembler_data->action_into_binary_map, _do_nothing);
	hash_table_free(assembler_data->symbol_table, _free_data);

	string_vector_free(assembler_data->warning_founds);
	string_vector_free(assembler_data->error_founds);

	queue_destroy(assembler_data->parsed_instruction_lines, _free_data);
	queue_destroy(assembler_data->extern_symbols, _free_data);

	free(assembler_data);

	return SUCCESS;
}

void clear_assembler_data(assembler_data_t *assembler_data)
{
	/* in case assembler_data is null*/
	if (assembler_data == NULL)
	{
		return;
	}
	hash_table_clear(assembler_data->symbol_table, _free_data);
	string_vector_clear(assembler_data->error_founds);
	string_vector_clear(assembler_data->warning_founds);
	queue_clear(assembler_data->parsed_instruction_lines, _free_data);
	queue_clear(assembler_data->extern_symbols, _free_data);

	assembler_data->IC = 0; /* (instruction counter) */
	assembler_data->DC = 0; /* (data counter) */
	assembler_data->line_count = 1;
}

int assembler_first_pass(assembler_data_t *assembler_data, FILE *input_file, HashTable *macro_table)
{
	enum line_type line_type = 0;
	int found_symbol = FALSE;
	char line[MAX_LINE_SIZE];
	char log_error[MAX_LINE_SIZE];

	memset(line, NEWLINE_CHARACTER, MAX_LINE_SIZE);

	while (fgets(line, MAX_LINE_SIZE, input_file) != NULL)
	{
		if ( ERR == is_line_too_long(line, assembler_data->line_count, assembler_data->warning_founds))
		{
			return FUNC_ERR;
		}

		line_type = identify_line_type(line);

 		if (is_symbol(line))
		{
			found_symbol = TRUE;
		}
		/* execute handler for line according to line_type */
		if (line_type < MAX_LINE_TYPE)
		{
			assembler_data->line_handlers_arr[line_type](line, assembler_data, found_symbol, macro_table);
		}
		else 
		{
			sprintf(log_error, "line type was not identified in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_LOG(log_error, assembler_data->error_founds);
		}
		found_symbol = FALSE;
		assembler_data->line_count++;
		memset(line, NEWLINE_CHARACTER, MAX_LINE_SIZE);

	}

	add_value_to_all_data_symbol(assembler_data);

	if (string_vector_get_size(assembler_data->error_founds) > 0)
	{
		return USER_ERR;
	}
	return SUCCESS;
}
