/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  line_handler.c
 * @brief This file contains the implementation of the line handler functions.
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

/* Initialize handler functions for each type of line*/
void init_line_handlers_arr(assembler_data_t *assembler_data)
{
	assembler_data->line_handlers_arr[LINE__EMPTY] = &handle_comment_or_empty_line;
	assembler_data->line_handlers_arr[LINE__COMMENT] = &handle_comment_or_empty_line;
	assembler_data->line_handlers_arr[LINE__DIRECTIVE] = &handle_directive_line;
	assembler_data->line_handlers_arr[LINE__INSTRUCTION] = &handle_instruction_line;
	assembler_data->line_handlers_arr[LINE__CONST_DEFINE] = &handle_const_defined_line;
}

int handle_const_defined_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table)
{
	const_define_line_t *const_define_line = NULL;
	int ret_val = SUCCESS;
	/* process line data and validating syntax*/
	ret_val = parse_const_define_line(line, macro_table, assembler_data, &const_define_line);
	if (ret_val == USER_ERR)
	{
		/* found error, therefore no point to push to symbol table */
		free(const_define_line);
		return ERR;
	}
	else if (ret_val == FUNC_ERR)
	{
		/* found error, therefore no point to push to symbol table */
		return FUNC_ERR;
	}
	if (SUCCESS != init_and_add_symbol_table(assembler_data, const_define_line->const_name, MEM_TYPE__DEFINE, const_define_line->const_value, 0))
	{
		return ERR;
	}
	free(const_define_line);
	return SUCCESS;
}

int handle_comment_or_empty_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table)
{
	UNUSED(line);
	UNUSED(assembler_data);
	UNUSED(found_symbol);
	UNUSED(macro_table);

	return SUCCESS;
}

int handle_directive_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table)
{
	directive_line_t *directive_line = NULL;
	enum symbol_location symbol_location;
	short curr_DC = assembler_data->DC;
	char data[MAX_LABEL_SIZE];
	int ret_val = SUCCESS;

	UNUSED(curr_DC);
	UNUSED(symbol_location);
	UNUSED(data);

	ret_val = parse_and_set_directive_line(line,  assembler_data, macro_table, &directive_line);
	if (ret_val != SUCCESS)
	{
		if (ret_val == USER_ERR)
		{
			/* found error, therefore no point to push to symbol table */
			free(directive_line);
		}
		return ret_val;
	}
	if (directive_line->directive_type == DIR__DATA ||
		directive_line->directive_type == DIR__STRING)
	{
		if (found_symbol)
		{
			if (SUCCESS != init_and_add_symbol_table(assembler_data, directive_line->label, MEM_TYPE__DATA, curr_DC, directive_line->num_elements))
			{
				return USER_ERR;
			}
		}
	}
	else
	{
		symbol_location = directive_line->directive_type == DIR__ENTRY ? SYMBOL__ENTRY : SYMBOL__EXTERN;
		init_and_add_location_to_symbol_table(assembler_data, directive_line->data_ptr, symbol_location);
	}
	free(directive_line);

	return SUCCESS;
}

int handle_instruction_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table)
{
	int i = 0;
	instruction_line_t *instruction_line = NULL;
	short instruction_code_first_word = 0;
	short curr_IC = 0;
	short addressing_type = 0;
	int ret_val = SUCCESS;

	ret_val = parse_instruction_line(line, assembler_data, &instruction_line, macro_table);
	if (ret_val == FUNC_ERR)
	{
		/* allocation failed, need to return */
		return FUNC_ERR;
	}
	else if(ret_val == USER_ERR) 
	{
		instruction_line->L = -1; /* mark this line as failed to parse line */
		if (SUCCESS != queue_enqueue(assembler_data->parsed_instruction_lines, instruction_line))
		{
			LOG(ERROR, PARAMS, "failed to enqueue instruction line, exiting ...");
			return FUNC_ERR;;
		}
		return USER_ERR;
	}
	if (SUCCESS != queue_enqueue(assembler_data->parsed_instruction_lines, instruction_line))
	{
		LOG(ERROR, PARAMS, "failed to enqueue instruction line, exiting ...");
		return FUNC_ERR;;
	}

	if (found_symbol)
	{
		if (SUCCESS != init_and_add_symbol_table(assembler_data, instruction_line->label, MEM_TYPE__INSTRUCTION, get_IC_value(assembler_data), 0))
		{
			return FUNC_ERR;
		}
	}

	/* build first word of instruction code */
	instruction_line->L = calc_num_word_for_instruction_code(instruction_line);
	curr_IC = assembler_data->IC;
	if (allocate_mem_for_instruction_code(assembler_data, instruction_line->L) != SUCCESS)
	{
		LOG(ERROR, PARAMS, "failed to allocate memory for instruction code, exiting ...");
		return ERR;
	}
	instruction_code_first_word = build_first_word(instruction_line);
	set_first_word(assembler_data, curr_IC, instruction_code_first_word);

	for (i = 0; i < MAX_NUM_OPERAND; ++i)
	{
		if (instruction_line->operand_data_arr[i].type != OPERAND_TYPE__NO_OPERAND)
		{
			addressing_type = instruction_line->operand_data_arr[i].addressing_type;
			if (addressing_type != ADDRESSING__DIRECT)
			{
				/* can't fail because memory was already allocated */
				curr_IC = build_word_and_set_in_memory(assembler_data, instruction_line, curr_IC + 1, i);
			}
			else if (addressing_type != ADDRESSING__DIRECT_REGISTER)
			{
				++curr_IC;
			}
		}

	}

	return SUCCESS;
}


int identify_line_type(char *line)
{
	int line_type = -1;
	/*comment line*/
	if (identify_comment_line(line))
	{
		LOG(INFO, PARAMS, "comment line found in identify_line_type function");
		line_type = LINE__COMMENT;
	}

	/*empty line*/
	else if (identify_empty_line(line))
	{
		LOG(INFO, PARAMS, "empty line found in identify_line_type function");
		line_type = LINE__EMPTY;
	}

	/*const define line*/
	else if (identify_const_define_line(line))
	{
		LOG(INFO, PARAMS, "const define line found in identify_line_type function");
		line_type = LINE__CONST_DEFINE;
	}

	/*directive line*/
	else if (identify_directive_line(line) == FOUND)
	{
		LOG(INFO, PARAMS, "directive line found in identify_line_type function");
		line_type = LINE__DIRECTIVE;
	}

	/*instruction line*/
	else if (identify_instruction_line(line) == FOUND)
	{
		LOG(INFO, PARAMS, "instruction line found in identify_line_type function");
		line_type = LINE__INSTRUCTION;
	}

	return line_type;
}

int identify_comment_line(char *line)
{
	if (line[0] == COMMENT_LINE_PREFIX)
	{
		return FOUND;
	}
	return NOT_FOUND;
}

int identify_empty_line(char *line)
{
	int i = 0;
	while (isspace(line[i])&& line[i]  != NEWLINE_CHARACTER )
	{
		i++;
	}

	if ( line[i] == NEWLINE_CHARACTER)
	{
		return FOUND;
	}
	return NOT_FOUND;
}

int identify_const_define_line(char *line)
{
	char *const_define_ptr = strstr(line, CONST_DEFINE_LINE);

	if(const_define_ptr != NULL)  
	{
		if(memchr(line, NEWLINE_CHARACTER, const_define_ptr - line) == NULL )/*validating that const_define_ptr is before the newline character*/
		{
			return FOUND;
		}
	}
	return NOT_FOUND;
}

int identify_directive_line(char *line)
{
	int i =0 ;
	while( isspace(*line))
	{
		++line;
	}

	if (USER_ERR != find_directive_line_type_from_begin(line))
	{
		return FOUND;
	}

	/*in case there's a label before the directive type*/
	while( !isspace(line[i]))
	{
		++i;
	}
	while( isspace(line[i]))
	{
		++i;
	}

	if (USER_ERR != find_directive_line_type_from_begin(line+i))
	{
		return FOUND;
	}
	return NOT_FOUND;
}

int identify_instruction_line(char *line)
{
	return find_instruction(line);
}
