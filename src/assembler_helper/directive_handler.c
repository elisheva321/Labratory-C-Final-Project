/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  directive_handler.c
 * @brief This file contains the implementation of the directive line handler functions.
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

int find_directive_line_type_from_begin(const char *line)
{
	if ('.' == *line)
	{
		++line;

		if (strncmp(line, DIR_LINE_DATA, strlen(DIR_LINE_DATA)) == 0)
		{
			return DIR__DATA;
		}
		else if (strncmp(line, DIR_LINE_STRING, strlen(DIR_LINE_STRING)) == 0)
		{
			return DIR__STRING;
		}
		else if (strncmp(line, DIR_LINE_ENTRY, strlen(DIR_LINE_ENTRY)) == 0)
		{
			return DIR__ENTRY;
		}
		else if (strncmp(line, DIR_LINE_EXTERN, strlen(DIR_LINE_EXTERN)) == 0)
		{
			return DIR__EXTERN;
		}
	}
	return USER_ERR;
}

int find_directive_line_type(const char *line)
{
	char *line_runner = (char *)line;
	if (line == NULL)
	{
		return USER_ERR;
	}
	line_runner = strstr(line, DIR_LINE_DATA);
	if (line_runner != NULL && *(line_runner - 1) == DIR_LINE_PREFIX)
	{
		return DIR__DATA;
	}
	line_runner = strstr(line, DIR_LINE_STRING);
	if (line_runner != NULL && *(line_runner - 1) == DIR_LINE_PREFIX)
	{
		return DIR__STRING;
	}
	line_runner = strstr(line, DIR_LINE_ENTRY);
	if (line_runner != NULL && *(line_runner - 1) == DIR_LINE_PREFIX)
	{
		return DIR__ENTRY;
	}
	line_runner = strstr(line, DIR_LINE_EXTERN);
	if (line_runner != NULL && *(line_runner - 1) == DIR_LINE_PREFIX)
	{
		return DIR__EXTERN;
	}
	return USER_ERR;
}


int parse_and_set_directive_line(const char *line, assembler_data_t *assembler_data, HashTable *macro_table, directive_line_t **directive_line)
{
	int i = 0;
	char directive_name[MAX_LABEL_SIZE];
	int directive_name_size = 0;
	char log_error[MAX_LINE_SIZE];
	char label_name[MAX_LINE_SIZE];
	char *line_runner = (char *)line;
	int ret_val;
	int directive_type;

	*directive_line = (directive_line_t *)malloc(sizeof(directive_line_t));
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(*directive_line, "Memory allocation for directive_line failed", FUNC_ERR);

	while( isspace(*line_runner))/* skipping possible white spaces*/
	{
		++line_runner;
	}

	/* in case the line contains a label*/
	if( line_runner[0] != DIR_LINE_PREFIX)
	{
		ret_val = handle_label(&line_runner, label_name, LINE__INSTRUCTION, macro_table, assembler_data);
		if ( ret_val != SUCCESS)
		{
				return ret_val;
		}
		strcpy((*directive_line)->label, label_name);
	}
	else
	{
		(*directive_line)->label[0] = NULL_TERMINATOR;
	}

	for (i = 0; !isspace(line_runner[i]); i++)
	{
		++directive_name_size;
	}
	strncpy(directive_name, line_runner + 1, directive_name_size - 1); /* remove the char '.' from the name*/
	directive_name[directive_name_size - 1] = NULL_TERMINATOR;

	directive_type = find_directive_line_type_from_begin(line_runner);
	
	if(directive_type < 0)
	{
		sprintf(log_error, "directive type in line: %d was not identified", assembler_data -> line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data ->error_founds);
	}

	switch (directive_type) 
	{
    	case DIR__DATA:
		{
			return set_directive_type_data(line_runner, directive_name, assembler_data, directive_line);
		}
		case DIR__STRING:
		{
			return set_directive_type_string(line_runner, directive_name, assembler_data, directive_line);
		}

		case DIR__ENTRY:
		{
			return set_directive_type_entry(line_runner, directive_name, assembler_data, directive_line);
		}

		case DIR__EXTERN:
		{
			return set_directive_type_extern(line_runner, directive_name, assembler_data, directive_line);
		}

	}

	return SUCCESS;
}

int allocate_memory_and_set_data_for_str(assembler_data_t *assembler_data, directive_line_t *directive_line, const char *line)
{
	short curr_DC = assembler_data->DC;
	short i = 0;

	if (_get_sum_memory(assembler_data) + directive_line->num_elements >= MEMORY_SIZE)
	{
		LOG(ERROR, PARAMS, "failed to allocate memory in data segment for symbol %s, exiting", directive_line->label);
		return USER_ERR;
	}

	for (i = 0; i < directive_line->num_elements - 1; ++i)
	{
		assembler_data->data_memory[curr_DC] = directive_line->data_ptr[i];
		++curr_DC;

	}
	assembler_data->data_memory[curr_DC] = NULL_TERMINATOR;
	assembler_data->DC += directive_line->num_elements;

	return SUCCESS;
}

short allocate_mem_for_data(assembler_data_t *assembler_data, directive_line_t *directive_line, int num_elements)
{
	if (_get_sum_memory(assembler_data) + num_elements >= MEMORY_SIZE)
	{
		LOG(ERROR, PARAMS, "failed to allocate memory in data segment for symbol %s, exiting", directive_line->label);
		return ERR;
	}
	assembler_data->DC += num_elements;
	return SUCCESS;
}

int data_array_validation_and_set(char *line, assembler_data_t *assembler_data, int curr_DC)
{
	char log_error[MAX_LINE_SIZE];
	char potential_const_define[MAX_LABEL_SIZE];
	int potential_const_define_size = 0;
	char number_value[MAX_INTEGER_LENGTH];
	int number_size = 0;
	int num_to_save = 0;
	short elements_counter = 0;
	int i = 0;

	/* This loop runs as long as it doesn't reach the end of the line*/
	for(elements_counter = 0 ; line[0] != NEWLINE_CHARACTER ; ++elements_counter)
	{
		if ((*line) != ',' && elements_counter != 0)
		{
		    sprintf(log_error, "missing comma between two elements in the array. found in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error , ERR, assembler_data->error_founds);
		}

		if (elements_counter != 0)
		{
			++line; /*skip the comma*/
		}
		if(skip_whitespace(&line, assembler_data->line_count , assembler_data->error_founds) == ERR) /*skip possible whitespaces before a member of the array*/
		{
			sprintf(log_error, "missing data array elements in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
		}

		/* in case a member of the array is a const*/
		if (isalpha(*line))
		{
			strncpy(potential_const_define, line , MAX_LABEL_SIZE -1);
			while(!isspace(*line) && (*line != COMMA))
			{
				++potential_const_define_size;
				++line;
			}
			potential_const_define[potential_const_define_size] = NULL_TERMINATOR;
			potential_const_define_size = 0;
			if( find_symbol_value(&num_to_save , potential_const_define, assembler_data->symbol_table) == ERR)
			{
				sprintf(log_error, "invalid const name: %s. found in line: %d", potential_const_define, assembler_data->line_count);
				PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
			}	
		}

		else if (isdigit(*line) || (*line) == '-')
		{
			while(isdigit(*line) || (*line) == '-')
			{
				++number_size;
				++line;
			}
			if(!isspace(*line) && (*line) != COMMA)
			{
				sprintf(log_error, "invalid char for an array: '%c' found in line: %d", (*line) , assembler_data->line_count);
				PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
			}
			if(number_size > MAX_INTEGER_LENGTH)
			{
				sprintf(log_error, "Number exceeds memory capacity. found in line: %d", assembler_data->line_count);
				PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
			}
			/* line -= number_size;*/
			strncpy(number_value, line - number_size , number_size);
			number_value[number_size] = NULL_TERMINATOR;
		    num_to_save = atoi(number_value);

			if(num_to_save > MAX_INTEGER_VALUE||num_to_save < MIN_INTEGER_VALUE)
			{
				sprintf(log_error, "Number: %d exceeds memory capacity. found in line: %d", num_to_save , assembler_data->line_count);
				PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
			}
		}

		else 
		{
		    sprintf(log_error, "invalid syntax for an array. found in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}

		assembler_data->data_memory[curr_DC] = num_to_save;
		++curr_DC;
		number_size = 0;
		for(i = 0; isspace(line[i]) && line[i] !=  NEWLINE_CHARACTER ; i++);
		line += i;
	}
	return SUCCESS;
}


void deal_with_more_than_two_quotation_marks(char *data_content, int *i, int *data_content_size)
{
	for (; data_content[*i] != NEWLINE_CHARACTER; (*i)++)
	{
		if (strchr(data_content + (*i), STRING_PREFIX_OR_POSTFIX) == NULL)
		{
			return;
		}
		(*data_content_size)++;
	}
}


int set_directive_type_data(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line)
{
	int i = 0;
	int curr_DC = assembler_data->DC;
	char log_error[MAX_LINE_SIZE];

	if(strlen(directive_name) != strlen(DIR_LINE_DATA))
	{
		sprintf(log_error, "no space between .data and data content, found in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}

	(*directive_line)->directive_type = DIR__DATA;
	line += strlen(directive_name) + 1; 

	for (i = 0; !isdigit(line[i]) && line[i] != NEWLINE_CHARACTER; i++);

	(*directive_line)->num_elements = count_commas(line) + 1;
	(*directive_line)->data_ptr = line + i;

	if (SUCCESS != allocate_mem_for_data(assembler_data,(*directive_line), (*directive_line)->num_elements))
	{
		LOG(ERROR, PARAMS, "allocate_mem_for_data failed at set_directive_type_data");
		return ERR;
	}
	return data_array_validation_and_set(line, assembler_data, curr_DC);
}

int set_directive_type_string(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line)
{
	int i = 0;
	int data_content_size = 0;
	char log_error[MAX_LINE_SIZE];

	if(strlen(directive_name) != strlen(DIR_LINE_STRING))
	{
		sprintf(log_error, "no space between .string and string content, found in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}

	(*directive_line)->directive_type = DIR__STRING;
	line += strlen(directive_name) + 1;
		
	if (skip_whitespace(&line, assembler_data->line_count , assembler_data->error_founds)== ERR)
	{
		sprintf(log_error, "reached end of line before string definition, in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	} 

	if (line[0] != STRING_PREFIX_OR_POSTFIX)
	{
		sprintf(log_error, "missing \" at the beginning of the string, found in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}
	
	++line;
	for (i = 0; line[i] != STRING_PREFIX_OR_POSTFIX && line[i] != NEWLINE_CHARACTER; i++)
	{
		data_content_size++;
		if(!isprint(line[i]))
		{
			sprintf(log_error, "invalid characters for string, found in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
		}
	}

	if (line[i] == NEWLINE_CHARACTER)
	{
		sprintf(log_error, "missing \" at the end of the string, found in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}

	deal_with_more_than_two_quotation_marks((line + i), &i, &data_content_size);
	++i; /* skipping the " at the end of the string*/

	for ( ; line[i] != NEWLINE_CHARACTER; i++)
	{
		if (!isspace(line[i]))
		{
			sprintf(log_error, "non whitespaces char after the string, found in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
		}
	}
		
	(*directive_line)->data_ptr = line;
	(*directive_line)->num_elements = data_content_size + 1;
	if (SUCCESS !=  allocate_memory_and_set_data_for_str(assembler_data, *directive_line, line))
	{
		LOG(ERROR, PARAMS, "allocate_memory_and_set_data_for_str failed at set_directive_type_string");
		return ERR;
	}

	return SUCCESS;
}


int set_directive_type_entry(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line)
{
	char log_error[MAX_LINE_SIZE];
	char log_warning[MAX_LINE_SIZE];

	if(strlen(directive_name) != strlen(DIR_LINE_ENTRY))
	{
		sprintf(log_error, "no space between .entry and entry content, found in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}

	if((*directive_line)->label[0] != NULL_TERMINATOR)
	{
		sprintf(log_warning, "line: %d Label detected in entry line",  assembler_data->line_count);
		PUSH_WARNING(log_warning, assembler_data->warning_founds);
	}

	line += strlen(directive_name)+1;
	if (skip_whitespace(&line, assembler_data->line_count , assembler_data->error_founds) == ERR)
	{
		sprintf(log_error, "missing entry label name in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}

	(*directive_line)->directive_type = DIR__ENTRY;
	(*directive_line)->data_ptr = line;
	(*directive_line)->num_elements = 1;

	return SUCCESS;
}

int set_directive_type_extern(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line)
{
	char log_error[MAX_LINE_SIZE];
	char log_warning[MAX_LINE_SIZE];

	if( strlen(directive_name) != strlen(DIR_LINE_EXTERN) )
	{
		sprintf(log_error, "no space between .extern and extern content, found in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}

	if((*directive_line)->label[0] != NULL_TERMINATOR)
	{
		sprintf(log_warning, "Label detected in a directive extern line, line number: %d",  assembler_data->line_count);
		PUSH_WARNING(log_warning, assembler_data->warning_founds);
	}

	line += strlen(directive_name)+1;
	if (skip_whitespace(&line, assembler_data->line_count , assembler_data->error_founds) == ERR)
	{
		sprintf(log_error, "missing extern label name in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
	}
	
	(*directive_line)->directive_type = DIR__EXTERN;
	(*directive_line)->data_ptr = line;
	(*directive_line)->num_elements = 1;
	
	return SUCCESS;
}

