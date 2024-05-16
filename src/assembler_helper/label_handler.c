/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  label_handler.c
 * @brief This file contains the implementation of the label handler functions.
 *
 *********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h> 

#include "../pre_processor/preprocessor_consts.h"
#include "assembler_helper.h"
#include "../utils/utils.h"
#include "../logger/logger.h"
#include "../general_const/general_const.h"
#include "../hash_table/hash_table.h"
#include "../string_vector/string_vector.h"

int validate_label_name(char *label_name, int *label_size, int line_type, HashTable *macro_table, assembler_data_t *assembler_data)
{
	int ret_val = SUCCESS;

	/* in case label's name doesn't end with ':'*/
	ret_val = validate_last_char_label_name(label_name, label_size, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);

	/* in case the first char of the label isn't a letter*/
	ret_val = validate_first_char_label_name(label_name, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);


	/* in case the label name contains characters that aren't letters or numbers */
	ret_val = validate_label_name_characters(label_name, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);


	/* in case a label has a register's name*/
	ret_val = validate_is_label_name_register(label_name, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);


	/* in case the label's name is an instruction name*/
	ret_val = validate_is_label_name_instruction(label_name, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);


	/* in case the label's name is a const define name*/
	ret_val = validate_is_label_name_const_define(label_name, assembler_data->symbol_table, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);


	/* in case there's two labels with the same name */
	ret_val = validate_is_label_name_dupe(label_name, line_type, assembler_data->symbol_table, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);

	/* in case label's name is a macro name */
	ret_val = validate_is_label_name_macro(label_name, macro_table, assembler_data->line_count, assembler_data->error_founds);

	/*in case label's name is a directive name */
	ret_val = validate_is_label_name_directive(label_name, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);

	
	/*in case label's name is a macro defenition - mcr or endmcr */
	ret_val = validate_is_label_name_macro_defenition(label_name, assembler_data->line_count, assembler_data->error_founds);
	CHECK_RET_VAL_AND_RETURN(ret_val);

	return SUCCESS;
}

int validate_last_char_label_name(char *label_name, int *label_size, int line_count, StringVector *error_founds)
{
	char log_error[MAX_LINE_SIZE];

	if (label_name[(*label_size) - 1] != LABEL_POSTFIX)
	{
		sprintf(log_error, "Invalid label name: %s, label name doesn't end with ':'. found in line: %d", label_name, line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR, error_founds)
	}
	label_name[(*label_size) - 1] = NULL_TERMINATOR; /*removing the character ':' from the string because it is not part of the label name*/
	return SUCCESS;
}

int validate_first_char_label_name(char *label_name,  int line_count, StringVector *error_founds)
{
	char log_error[MAX_LINE_SIZE];

	if (!isalpha(label_name[0]))
	{
		sprintf(log_error, "Invalid label name: %s, should start with a letter. line: %d", label_name, line_count);
		PUSH_ERROR_AND_RETURN( log_error , USER_ERR, error_founds);
	}
	return SUCCESS;
}


int validate_label_name_characters(char *label_name,  int line_count, StringVector *error_founds)
{
	char log_error[MAX_LINE_SIZE];
	int i;

	for( i = 1 ; i < strlen(label_name) ; ++i)
	{
		if (!isalpha(label_name[i]) && !isdigit(label_name[i]))
		{
			sprintf(log_error, "character '%c' is invalid for a label name. found in line: %d", label_name[i], line_count);
			PUSH_ERROR_AND_RETURN( log_error , USER_ERR, error_founds);
		}
	}
	
	return SUCCESS;
}

int validate_is_label_name_register(char *label_name, int line_count, StringVector *error_founds)
{
	int i = 0;
	char log_error[MAX_LINE_SIZE];

	for (i = 0; i < NUM_OF_REGISTERS; i++)
	{
		if (strcmp(label_name, REGISTERS_NAMES[i]) == 0)
		{
			sprintf(log_error, "label: %s is a register's name. found in line: %d", label_name, line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, error_founds);
		}
	}
	return SUCCESS;
}


int validate_is_label_name_macro_defenition(char *label_name, int line_count, StringVector *error_founds)
{
	char log_error[MAX_LINE_SIZE];

	if (strcmp(label_name, MACRO_PREFIX) == 0 || strcmp(label_name, MACRO_POSTFIX) == 0)
	{
			sprintf(log_error, "label: %s is a macro defenition. found in line: %d", label_name, line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, error_founds);
	}
	return SUCCESS;
}


int validate_is_label_name_instruction(char *label_name, int line_count, StringVector *error_founds)
{
	int i = 0;
	char log_error[MAX_LINE_SIZE];

	for (i = 0; i < NUM_INSTRUCTIONS; i++)
	{
		if (strncmp(label_name, INSTRUCTIONS_NAMES[i], INSTRUCTION_LENGTH) == 0)
		{
			sprintf(log_error, "Invalid label name: %s, label name is an instruction. found in line: %d", label_name, line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, error_founds);
		}
	}
	return SUCCESS;
}

int validate_is_label_name_const_define(char *label_name, HashTable *symbol_table, int line_count, StringVector *error_founds)
{
	symbol_t *symbol = NULL;
	char log_error[MAX_LINE_SIZE];

	symbol = hash_table_lookup(symbol_table, label_name); 
	if (symbol != NULL)
	{
		if (symbol->memory_type == MEM_TYPE__DEFINE)
		{
			sprintf(log_error, "Invalid label name: %s, label name is a const define name. found in line: %d", label_name, line_count);
			PUSH_ERROR_AND_RETURN( log_error , USER_ERR, error_founds);
		}
	}
	return SUCCESS;
}

int validate_is_label_name_dupe(char *label_name, int line_type, HashTable *symbol_table, int line_count, StringVector *error_founds)
{
	symbol_t *symbol = hash_table_lookup(symbol_table, label_name);
	char log_error[MAX_LINE_SIZE];

	if (symbol != NULL)
	{
		if (symbol->memory_type == MEM_TYPE__DATA && line_type == LINE__DIRECTIVE)
		{
			sprintf(log_error, "Invalid label name: %s, two labels can't have the same name. found in line: %d", label_name, line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, error_founds);
		}
	}
	return SUCCESS;
}

int validate_is_label_name_macro(char *label_name, HashTable *macro_table, int line_count, StringVector *error_founds)
{
	StringVector *macro_data_content = hash_table_lookup(macro_table, label_name);
	char log_error[MAX_LINE_SIZE];

	if (macro_data_content != NULL)
	{
		sprintf(log_error, "Invalid label name: %s, label name can't be macro name. found in line: %d", label_name, line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, error_founds);
	}
	return SUCCESS;
}

int validate_is_label_name_directive(char *label_name, int line_count, StringVector *error_founds)
{
	int i = 0;
	char log_error[MAX_LINE_SIZE];

	if( label_name[0] == '.' )
	{
		++(*label_name); 
		for (i = 0; i < NUM_OF_DIRECTIVES; i++)
		{
			if (strcmp(label_name, DIRECTIVES_NAMES[i]) == 0)
			{
				sprintf(log_error, "label: %s is a directive name. found in line: %d", label_name, line_count);
				PUSH_ERROR_AND_RETURN( log_error, USER_ERR, error_founds);
			}
		}
	}
	return SUCCESS;
}


int validate_extern_label(char *label_name, assembler_data_t *assembler_data)
{
	char log_error[MAX_LINE_SIZE];
	symbol_t *symbol = NULL;
	symbol = hash_table_lookup(assembler_data->symbol_table, label_name);

	if (symbol != NULL)
	{
		sprintf(log_error, "line: %d label: %s already defined so can't be defined as extern.",assembler_data->line_count, label_name);
		PUSH_ERROR_AND_RETURN( log_error , USER_ERR, assembler_data->error_founds);
	}
	return SUCCESS;
}


int handle_label(char **line, char *label_name, int line_type, HashTable *macro_table, assembler_data_t *assembler_data)
{
	int ret_val = 0;
	char log_error[MAX_LINE_SIZE];
	char log_warning[MAX_LINE_SIZE];
	int label_size = 0;
	int i = 0;
	
	
	for (; !isspace((*line)[i]) && (*line)[i] != NEWLINE_CHARACTER; i++) /* getting label size*/
	{
		++label_size;
	}

	if( (*line)[i] == NEWLINE_CHARACTER)
	{
		sprintf(log_error, "invalid line syntax. found in line %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR,assembler_data->error_founds);
	}

	if(label_size > MAX_LABEL_SIZE) 
	{
		sprintf(log_warning, "label longer than 31 characters. found in line: %d", assembler_data->line_count);
		PUSH_WARNING(log_warning, assembler_data-> warning_founds);
	}
	
	strncpy(label_name, (*line), label_size);
	label_name[label_size] = NULL_TERMINATOR;

	ret_val = validate_label_name(label_name, &label_size, line_type, macro_table, assembler_data);      
	if ( ret_val != SUCCESS)
	{
		return ret_val;
	}

	*line += label_size;/* label_name was shorten but it doesn't affect the line*/
	if (skip_whitespace(line, assembler_data->line_count, assembler_data->error_founds) == ERR) /* skipping whitespaces between words*/
	{
		sprintf(log_error, "invalid line syntax. found in line %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN(log_error, USER_ERR,assembler_data->error_founds );
	}

	return SUCCESS;
}



int validate_is_label_name_entry_and_extern(char *label_name, directive_line_t *directive_line, assembler_data_t *assembler_data)
{
	symbol_t *symbol = NULL;
	char log_error[MAX_LINE_SIZE];

	symbol = hash_table_lookup(assembler_data->symbol_table, label_name);

	if (symbol != NULL)
	{
		if (symbol->symbol_location == SYMBOL__ENTRY && directive_line->directive_type == DIR__EXTERN)
		{
			sprintf(log_error, "line: %d label: %s was defined as entry and cant be defined as extern.", assembler_data->line_count, label_name);
			PUSH_ERROR_AND_RETURN( log_error , USER_ERR, assembler_data->error_founds);
		}
		else if (symbol->symbol_location == SYMBOL__EXTERN && directive_line->directive_type == DIR__ENTRY)
		{
			sprintf(log_error, "line: %d label: %s was defined as extern and cant be defined as entry", assembler_data->line_count, label_name);
			PUSH_ERROR_AND_RETURN(log_error, USER_ERR, assembler_data->error_founds);
		}
	}
	return VALID;
}



int validate_label_for_direct_addressing(char *operand_name)
{
	int i;

	if (!isalpha(operand_name[0]))
	{
		return ERR;
	}

	for (i = 1; i < strlen(operand_name); i++)
	{
		if (!isalpha(operand_name[i]) && !isdigit(operand_name[i]))
		{
			return ERR;
		}
	}

	for (i = 0; i < NUM_OF_REGISTERS; i++)
	{
		if (strcmp(operand_name, REGISTERS_NAMES[i]) == 0)
		{
			return ERR;
		}
	}

	for (i = 0; i < NUM_INSTRUCTIONS; i++)
	{
		if (strncmp(operand_name, INSTRUCTIONS_NAMES[i], INSTRUCTION_LENGTH) == 0)
		{
			return ERR;
		}
	}

	return SUCCESS;
}
