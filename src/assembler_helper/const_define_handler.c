/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  const_define_handler.c
 * @brief This file contains the implementation of the const define line handler functions.
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


int parse_const_define_line(const char *line, HashTable *macro_table, assembler_data_t *assembler_data, const_define_line_t **const_define_line)
{
	int i = 0;
	int const_name_size = 0;
	int const_value_size = 0;
	char const_value_content[MAX_INTEGER_LENGTH + 1];
	char log_error[MAX_LINE_SIZE];
	char *line_runner = (char *)line;

	*const_define_line = (const_define_line_t *)malloc(sizeof(const_define_line_t));
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(*const_define_line, "Memory allocation for const_define_line failed", FUNC_ERR);


	while( isspace(*line_runner))/*skipping possible whitespaces*/
	{
		++line_runner;
	}

	line_runner += strlen(CONST_DEFINE_LINE);

	if (!isspace(line_runner[0])) /* validating that there's a space between '.define' and const define name*/
	{
		sprintf(log_error, "error found in line: %d, no space between '.define' and const define name", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	if (skip_whitespace(&line_runner, assembler_data->line_count , assembler_data->error_founds) == ERR) /*skipping whitespaces between words*/
	{
		sprintf(log_error, "missing const define defenition in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	strcpy((*const_define_line)->const_name, line_runner);

	for (; !isspace(line_runner[i]) && line_runner[i] != NEWLINE_CHARACTER && line_runner[i] != '='; i++)
	{
		const_name_size++;
	}
	line_runner += const_name_size;

	if (skip_whitespace(&line_runner, assembler_data->line_count , assembler_data->error_founds) == ERR) /*skipping whitespaces between words*/
	{
		sprintf(log_error, "missing const define value in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	(*const_define_line)->const_name[const_name_size] = NULL_TERMINATOR;

	if (*line_runner != '=')
	{
		sprintf(log_error, "error found in line: %d, no '=' character between the const name and value", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}
	/*skip = */
	++line_runner;
	if (skip_whitespace(&line_runner, assembler_data->line_count , assembler_data->error_founds) == ERR) /* skipping whitespaces between words */
	{
		sprintf(log_error, "missing const define value in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	strcpy(const_value_content, line_runner);

	if (*line_runner == '-') /*in case the const value is negative*/
	{
		/*increasing these variables by 1 before the loop, because line[i] isnt a digit and its not an error */
		++line_runner;
		++const_value_size;
	}

	for (; !isspace(*line_runner) && *line_runner != NEWLINE_CHARACTER; ++line_runner) /*validating that the const is a number*/
	{
		if (!isdigit(*line_runner))
		{
			sprintf(log_error, "error found in line: %d, const isn't a number", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
		const_value_size++;
	}
	const_value_content[const_value_size] = NULL_TERMINATOR;

	if (const_value_size > MAX_INTEGER_LENGTH)
	{
		sprintf(log_error, "error found in line: %d, number exceeds memory capacity", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	(*const_define_line)->const_value = atoi(const_value_content);

	if ((*const_define_line)->const_value > MAX_INTEGER_VALUE || (*const_define_line)->const_value < MIN_INTEGER_VALUE)
	{
		sprintf(log_error, "error found in line: %d, number exceeds memory capacity", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	for (; *line_runner != NEWLINE_CHARACTER; ++line_runner)
		; /*validating that the only characters after the const are whitespaces*/
	{
		if (!isspace(*line_runner))
		{
			sprintf(log_error, "error found in line: %d, invalid syntax", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
	}

	return SUCCESS;
}
