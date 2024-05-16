/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  symbol_handler.c
 * @brief This file contains the implementation of the symbol handler functions.
 * 
 *********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> 

#include "assembler_helper.h"
#include "../utils/utils.h"
#include "../logger/logger.h"
#include "../general_const/general_const.h"
#include "../hash_table/hash_table.h"
#include "../string_vector/string_vector.h"

void init_symbol_data(symbol_t *symbol, const char *label_name, enum memory_type memory_type, short symbol_value, short num_elements)
{
	symbol->memory_type = memory_type;
	symbol->symbol_value = symbol_value;
	strcpy(symbol->symbol_name, label_name);
	symbol->symbol_location = -1;
	symbol->num_elements = num_elements;
}

void update_symbol_data(symbol_t *symbol, const char *label_name, enum memory_type memory_type, short symbol_value, short num_elements)
{
	symbol->memory_type = memory_type;
	symbol->symbol_value = symbol_value;
	symbol->num_elements = num_elements;
}

void init_symbol_location_data(symbol_t *symbol, const char *label_name, enum symbol_location symbol_location)
{
	symbol->symbol_location = symbol_location;
	symbol->symbol_value = (symbol_location == SYMBOL__EXTERN) ? 0 : -1;
	strcpy(symbol->symbol_name, label_name);
}

void update_symbol_location_data(symbol_t *symbol, enum symbol_location symbol_location)
{
	symbol->symbol_location = symbol_location;
}


int init_and_add_symbol_table(assembler_data_t *assembler_data, const char *label_name, enum memory_type memory_type, short symbol_value, short num_elements)
{
	symbol_t *symbol = hash_table_lookup(assembler_data->symbol_table, label_name);
	char log_error[MAX_LINE_SIZE];
	if (symbol)
	{
		if (symbol->symbol_value != -1)
		{
			sprintf(log_error, "line number: %d symbol %s already exists", assembler_data->line_count, label_name);
			PUSH_ERROR_AND_LOG( log_error , assembler_data->error_founds);
		}
		else if (symbol->symbol_location == SYMBOL__EXTERN)
		{
			sprintf(log_error, "line number: %d symbol %s defined as extern but definition is given in this file.", assembler_data->line_count, label_name);
			PUSH_ERROR_AND_LOG( log_error , assembler_data->error_founds);
		}
		else
		{
			/* just update existing symbol*/
			update_symbol_data(symbol, label_name, memory_type, symbol_value, num_elements);
		}
	}
	else
	{
		/* allocate memory for the new symbol*/
		symbol = (symbol_t *)malloc(sizeof(symbol_t));
		CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(symbol, "Memory allocation for symbol failed", FUNC_ERR);

		init_symbol_data(symbol, label_name, memory_type, symbol_value, num_elements);
		if (SUCCESS !=  hash_table_insert(assembler_data->symbol_table, label_name, symbol))
		{
			LOG(ERROR, PARAMS, "failed to insert symbol %s to symbol table, exiting ...", label_name);
			return USER_ERR;
		}
	}
	return SUCCESS;
}


int init_and_add_location_to_symbol_table(assembler_data_t *assembler_data, const char *label_name_from_line, enum symbol_location symbol_location)
{
	char log_error[MAX_LINE_SIZE];
	int i = 0;
	int label_size = 0;
	char label_name_temp[MAX_LINE_SIZE];
	symbol_t *symbol = NULL;
	
	memset(label_name_temp, 0, MAX_LINE_SIZE);

	for (i = 0; !isspace(label_name_from_line[i]); i++)
	{
		label_size++;
	}

	strncpy(label_name_temp, label_name_from_line, label_size);
	label_name_temp[label_size] = NULL_TERMINATOR;
	symbol = (symbol_t *)hash_table_lookup(assembler_data->symbol_table, label_name_temp);
	if (symbol)
	{
		if (symbol->symbol_location != -1)
		{
			/* symbol already exists, print error + LOG(for the user) */
			sprintf(log_error, "line number: %d symbol %s already exists", assembler_data->line_count, label_name_from_line);
			PUSH_ERROR_AND_LOG( log_error , assembler_data->error_founds);
		}
		else
		{
			/* just update existing symbol*/
			update_symbol_location_data(symbol, symbol_location);
		}
	}
	else
	{
		/* allocate memory for the new symbol*/
		symbol = (symbol_t *)malloc(sizeof(symbol_t));
		memset(symbol, 0, sizeof(symbol_t));
		CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(symbol, "Memory allocation for symbol failed", FUNC_ERR);

		init_symbol_location_data(symbol, label_name_temp, symbol_location);
		if (SUCCESS !=  hash_table_insert(assembler_data->symbol_table, label_name_temp, symbol))
		{
			LOG(ERROR, PARAMS, "failed to insert symbol %s to symbol table, exiting ...", label_name_temp);
			return USER_ERR;
		}
	}
	if (symbol_location == SYMBOL__ENTRY)
	{
		assembler_data->has_entry = TRUE;
	}
	return SUCCESS;
}


void add_value_to_data_symbol(const char *key, void *data, void *IC)
{
	symbol_t *symbol = (symbol_t *)data;
	if (symbol->memory_type == MEM_TYPE__DATA)
	{
		symbol->symbol_value += *(int *)IC;
	}
}


void add_value_to_all_data_symbol(assembler_data_t *assembler_data)
{
	hash_table_for_each(assembler_data->symbol_table, add_value_to_data_symbol, (void *)&(assembler_data->IC));
}


int is_symbol(const char *line)
{
	/* Determine the maximum allowed length of the tag */
	int max_length = strlen(line) < MAX_LABEL_SIZE ? strlen(line) : MAX_LABEL_SIZE;
	int i;

	/*  Check if the first character in the line is an alphabetic character */
	if (!isalpha(line[0]))
		return FALSE;

	/*  Iterate over each character in the line */
	for (i = 1; i < max_length; i++)
	{
		/*  Check if the character is a valid tag character (alphabetic or numeric) */
		if (!isalnum(line[i]))
		{
			/*  If the character is ':', return true */
			if (line[i] == ':')
				return TRUE;
			/*  Otherwise, the line does not start with a valid tag */
			else
				return FALSE;
		}
	}
	return FALSE;
}


int find_symbol_value(int *found_value , char *lookup_name, HashTable *symbol_table)
{
	symbol_t *symbol;
	symbol = hash_table_lookup(symbol_table, lookup_name);
	if (symbol == NULL)
	{
		return ERR;
	}

	if (symbol->memory_type != MEM_TYPE__DEFINE)
	{
		return ERR;
	}

	(*found_value) = symbol->symbol_value;
	return SUCCESS;
}
