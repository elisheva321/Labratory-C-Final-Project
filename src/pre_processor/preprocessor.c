/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota
 *
 * @file preprocessor_consts.h
 * @brief This file contains the implementations of the preprocessor functions.
 *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "preprocessor_consts.h"
#include "preprocessor.h"

#include "../general_const/general_const.h"
#include "../utils/utils.h"
#include "../logger/logger.h"
#include "../hash_table/hash_table.h"
#include "../string_vector/string_vector.h"
#include "../assembler_helper/assembler_helper.h"

/* =========================== internal functions declarations ================== */
int _pre_process_input(general_data_pre_process_t *general_data_pre_process, FILE *input_file, const char *output_file_name);

int _validate_macro_name(char *macro_name, int line_count, general_data_pre_process_t *data);

int _has_non_whitespace_characters_after_endmcr(const char *line, general_data_pre_process_t *general_data_pre_process);
void _get_end_line_index(int *end_line_index, const char *line);
void _get_first_word_from_line(const char *line, char *first_word, int *first_word_size, int *space_counter);
int _get_macro_name(char *macro_name, const char *line, int first_word_size, int space_counter, int *macro_name_size, StringVector *warning_founds);
void _remove_white_space_from_end_of_macro_name(int i, char *macro_name);

/*================================================================================ */
/* =========================== internal functions implementations ================= */

void _free_macro(void *ptr)
{
	string_vector_free(((StringVector *)ptr));
}

general_data_pre_process_t *init_pre_process_general_data()
{
	general_data_pre_process_t *general_data_pre_process = (general_data_pre_process_t *)malloc(sizeof(general_data_pre_process_t));
	LOG(DEBUG, PARAMS, "/================================================================================================================================*/\n");
	if (NULL == general_data_pre_process)
	{
		LOG(ERROR, PARAMS, "Memory allocation failed for general_data_pre_process");
		return NULL;
	}
	LOG(DEBUG, PARAMS, "initializing general data \n");

	general_data_pre_process->error_founds = string_vector_create();
	if (general_data_pre_process->error_founds == NULL)
	{
		return NULL;
	}
	general_data_pre_process->warning_founds = string_vector_create();
	if (general_data_pre_process->warning_founds == NULL)
	{
		return NULL;
	}
	general_data_pre_process->macro_table = hash_table_init();
	if (general_data_pre_process->macro_table == NULL)
	{
		LOG(ERROR, PARAMS, "Error initializing Hash table general_data_pre_process->macro_table");
		return NULL;
	}
	LOG(DEBUG, PARAMS, "finish initializing general data \n");

	return general_data_pre_process;
}
void clear_pre_process_data(general_data_pre_process_t *pre_process_data)
{
	string_vector_clear(pre_process_data->error_founds);
	string_vector_clear(pre_process_data->warning_founds);
	hash_table_clear(pre_process_data->macro_table, _free_macro);
}
void free_pre_process_data(general_data_pre_process_t *general_data_pre_process)
{
	string_vector_free(general_data_pre_process->error_founds);
	string_vector_free(general_data_pre_process->warning_founds);
	hash_table_free(general_data_pre_process->macro_table, _free_macro);

	free(general_data_pre_process);
}

void _get_first_word_from_line(const char *line, char *first_word, int *first_word_size, int *space_counter)
{
	int i = 0;

	(*first_word_size) = 0;
	/*in case the line starts with space*/
	while (isspace(line[i]))
	{
		line++;
		space_counter++;
	}

	while(!isspace(line[i]) && line[i] != NEWLINE_CHARACTER)
	{
		i++;
	}
	
	(*first_word_size) += i;
	strncpy(first_word, line, *first_word_size);
	first_word[*first_word_size] = NULL_TERMINATOR;

	LOG(DEBUG, PARAMS, "First word: %s\n", first_word);
}

int _get_macro_name(char *macro_name, const char *line, int first_word_size, int space_counter, int *macro_name_size, StringVector *warning_founds)
{
	char log_warning[MAX_LINE_SIZE];

	while(isspace(line[0]))
	{
		++line;
	}
	
	line +=first_word_size;

	/*skip white spaces*/
	while (isspace(line[0]))
	{
		line++;
	}

	if (*macro_name_size > MAX_LABEL_SIZE - 1)
	{
		sprintf(log_warning, "macro_name is longer than 31 characters");
		PUSH_WARNING(log_warning, warning_founds)
	}
	*macro_name_size = strlen(line);
	strcpy(macro_name, line);
	macro_name[*macro_name_size] = NULL_TERMINATOR;

	LOG(DEBUG, PARAMS, "macro_name: %s\n", macro_name);
	return SUCCESS;
}

void _get_end_line_index(int *end_line_index, const char *line)
{
	int i = 0;

	while (line[i] != '\0')
	{
		i++;
	}

	*end_line_index = i;
}

void _remove_white_space_from_end_of_macro_name(int i, char *macro_name)
{
	i = 0;

	while (macro_name[i] != '\0')
	{
		i++;
	}

	while (i > 0 && isspace(macro_name[i - 1]))
	{
		i--;
	}

	macro_name[i] = '\0';
}

int _validate_macro_name(char *macro_name, int line_count, general_data_pre_process_t *general_data_pre_process)
{
	/*in case the first char of the macro isn't a letter*/
	validate_first_char_label_name(macro_name, line_count, general_data_pre_process->error_founds);

	/*in case a macro has a register's name*/
	validate_is_label_name_register(macro_name, line_count, general_data_pre_process->error_founds);

	return SUCCESS;
}

int _has_non_whitespace_characters_after_endmcr(const char *line, general_data_pre_process_t *general_data_pre_process)
{
	char log_error[MAX_LINE_SIZE];
	while (strncmp(line, "endmcr", strlen("endmcr")) != 0)
		line++;

	/*placing the pointer after endmcr*/
	line += strlen("endmcr");

	while (*line != '\0')
	{
		if (!isspace(*line))
		{
			sprintf(log_error, "non whitespace letter after end mcr.\n");
			PUSH_ERROR_AND_RETURN(log_error, ERR, general_data_pre_process->error_founds);
		}
		line++;
	}
	return SUCCESS;
}

/*=============================================================================== */

int pre_process_input(general_data_pre_process_t *pre_process_data, FILE *input_file, const char *file_name, char *output_file_name)
{
	int ret = 0;

	LOG(DEBUG, PARAMS, "pre processing the %s file", file_name);

	get_output_file_name(file_name, output_file_name, PRE_PROCESSOR_FILE_EXTENSION);

	ret = _pre_process_input(pre_process_data, input_file, output_file_name);
	if (ret == ERR)
	{
		LOG(ERROR, PARAMS, "Error in pre processing the %s file", file_name);
	}
	else
	{
		LOG(INFO, PARAMS, "pre processing the %s file was successful", file_name);
	}
	return ret;
}
int _pre_process_input(general_data_pre_process_t *general_data_pre_process, FILE *input_file, const char *output_file_name)
{
	FILE *output_file = NULL;
	char line[MAX_LINE_SIZE];
	char first_word[MAX_LINE_SIZE];
	char macro_name[MAX_LINE_SIZE];
	int line_index = 1;
	int end_line_index = 0;
	int first_word_size = 0;
	int macro_name_size = 0;
	int macro_content_size = 0;
	int found_macro = FALSE;
	int i = 0;
	int space_counter = 0;
	StringVector *macro_data_content = NULL;


	output_file = fopen(output_file_name, "w");
	if (output_file == NULL)
	{
		LOG(ERROR, PARAMS, "Error opening output file %s", output_file_name);
		return ERR;
	}
	memset(line, NEWLINE_CHARACTER, MAX_LINE_SIZE);
	while (fgets(line, sizeof(line), input_file) != NULL)
	{
		line[MAX_LINE_SIZE - 1] = NEWLINE_CHARACTER;

		if (line[0] == COMMENT_LINE_PREFIX)
		{
			fprintf(output_file, "%s", line);
			++line_index;
			continue;
		}
		_get_first_word_from_line(line, first_word, &first_word_size, &space_counter);
		LOG(DEBUG, PARAMS, "line index : %d ", line_index);
		_get_end_line_index(&end_line_index, line);
		if (!found_macro)
		{
			macro_data_content = (StringVector *)hash_table_lookup(general_data_pre_process->macro_table, first_word);
		}
		/* first word of the line was found in the macro table, therefore the line should be replaced with the macro content */
		if (!found_macro && macro_data_content)
		{

			LOG(DEBUG, PARAMS, "Found macro! line index : %d, line: %s, first_word %s\n", line_index, line, first_word);
			macro_content_size = string_vector_get_size(macro_data_content);
			for (i = 0; i < macro_content_size; ++i)
			{

				fprintf(output_file, "%s", string_vector_at(macro_data_content, i));
			}
		}
		else if (0 == strcmp(first_word, MACRO_PREFIX))
		{
			if (_get_macro_name(macro_name, line, first_word_size, space_counter, &macro_name_size, general_data_pre_process->warning_founds) != SUCCESS)
			{
				return ERR;
			}
			if (_validate_macro_name(macro_name, line_index, general_data_pre_process) == SUCCESS)
			{
				_remove_white_space_from_end_of_macro_name(i, macro_name);
				LOG(DEBUG, PARAMS, "Found new macro! line index : %d, line: %s, first_word %s\n", line_index, line, first_word);
				found_macro = TRUE;
				macro_data_content = string_vector_create();
				if (macro_data_content == NULL)
				{
					return ERR;
				}
				/*macro_data->macro_content = vec;*/
				/*macro_name[0] = '\0';*/
			}
		}

		else if (found_macro && strstr(line, MACRO_POSTFIX) != NULL)
		{

			LOG(INFO, PARAMS, "inserting macro! macro_name : %s, macro_size %d", macro_name, string_vector_get_size(macro_data_content));
			hash_table_insert(general_data_pre_process->macro_table, macro_name, macro_data_content);
			found_macro = FALSE;
			macro_data_content = NULL;
		}

		else if (found_macro)
		{
			if (string_vector_push_back(macro_data_content, line) == ERR)
			{
				LOG(ERROR, PARAMS, "string_vector_push_back failed at pre_process_input");
				return ERR;
			}
		}
		else
		{
			fprintf(output_file, "%s", line);
		}

		++line_index;
		memset(line, NEWLINE_CHARACTER, MAX_LINE_SIZE);

	}

	fclose(input_file);
	fclose(output_file);

	LOG(DEBUG, PARAMS, "Finished preprossecor action. file %s is ready.", output_file_name);

	if (string_vector_get_size(general_data_pre_process->error_founds) > 0)
	{
		LOG(ERROR, PARAMS, "Errors found in preprocessor");
		remove(output_file_name);
		return USER_ERR;
	}
	return SUCCESS;
}
