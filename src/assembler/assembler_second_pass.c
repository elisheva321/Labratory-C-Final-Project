/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  assembler_second_pass.c
 * @brief This file contains the implementation 
 * of the second pass of the assembler.
 *
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "assembler.h"
#include "assembler_consts.h"
#include "../logger/logger.h"
#include "../general_const/general_const.h"
#include "../hash_table/hash_table.h"
#include "../dynamic_queue/dynamic_queue.h"
#include "../assembler_helper/assembler_helper.h"


/* --------------------- internal function declarations -------------------------*/
void _validate_entry_value(const char *key, void *data, void *assembler_data);
extern_user_t * _init_extern_user(const char *label_name, short address);
int _build_rest_of_instruction_code(assembler_data_t *assembler_data, instruction_line_t *instruction_line);

/*--------------------------------- internal function definitions -------------*/
void _validate_entry_value(const char *key, void *data, void *assembler_data)
{
    symbol_t *symbol = (symbol_t *)data;
	char log_error[MAX_LINE_SIZE];
	assembler_data_t *assembler_data_casted = (assembler_data_t *)assembler_data;
	if (symbol->symbol_location == SYMBOL__ENTRY)
	{
		if (symbol->symbol_value == -1)
		{
			sprintf(log_error, "entry value is not defined for symbol %s", key);

			if (string_vector_push_back(assembler_data_casted->error_founds, log_error) == ERR) 
		   	{
				LOG(ERROR, PARAMS, "string_vector_push_back failed");
			}
			LOG(ERROR, PARAMS, log_error);
		}
    }
}


extern_user_t * _init_extern_user(const char *label_name, short address)
{
	extern_user_t *extern_user = (extern_user_t *)malloc(sizeof(extern_user_t));
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(extern_user, "Memory allocation failed in _init_extern_user", NULL);

	extern_user->address = address;
	strcpy(extern_user->label_name, label_name);
	return extern_user;
}

int _build_rest_of_instruction_code(assembler_data_t *assembler_data, instruction_line_t *instruction_line)
{
	static const short ARE_EXTERNAL = 1;
	static const short ARE_RELOCATABLE = 2;
	short curr_IC = assembler_data->IC + 1;
	int i = 0;
	short word = 0;

	enum addressing_type addressing_type;
	short is_external;

	symbol_t *symbol;
	char log_error[MAX_LINE_SIZE];
	extern_user_t *extern_user;
	char *label_name;

	UNUSED(assembler_data);
	UNUSED(instruction_line);
	UNUSED(word);
	UNUSED(symbol);

	for (i = 0; i < MAX_NUM_OPERAND; ++i)
	{
		if (instruction_line->operand_data_arr[i].type == OPERAND_TYPE__NO_OPERAND)
		{
			break;
		}
		addressing_type = instruction_line->operand_data_arr[i].addressing_type;

		switch (addressing_type)
		{
		case ADDRESSING__DIRECT:
		{
			label_name = instruction_line->operand_data_arr[i].operand.name;
			symbol = (symbol_t *)hash_table_lookup(assembler_data->symbol_table, label_name);
			if (symbol == NULL)
			{
				sprintf(log_error, "undefined Symbol %s", instruction_line->operand_data_arr[i].operand.name);
				LOG(INFO, PARAMS, "%s", log_error);
				if (SUCCESS != string_vector_push_back(assembler_data->error_founds, log_error))
				{
					LOG(ERROR, PARAMS, "Memory allocation failed in _build_rest_of_instruction_code");
					return FUNC_ERR;
				}
				return USER_ERR;
			}

			is_external = symbol->symbol_location == SYMBOL__EXTERN;
			if (is_external && instruction_line->L != -1)
			{
				extern_user = _init_extern_user(label_name, curr_IC);
				queue_enqueue(assembler_data->extern_symbols, extern_user);
			}
			word = set_bits_short(word, is_external ? ARE_EXTERNAL : ARE_RELOCATABLE, 2, 0);

			/* set address of the symbol in the word*/
			word = set_bits_short(word, symbol->symbol_value, 12, 2);
			assembler_data->machine_binary_code[curr_IC] = word;
			break;
		}
		case ADDRESSING__DATA_INDEX:
		{
			label_name = instruction_line->operand_data_arr[i].operand.name;
			symbol = (symbol_t *)hash_table_lookup(assembler_data->symbol_table, instruction_line->operand_data_arr[i].operand.data_index.arr_name);
			if (symbol == NULL)
			{
				sprintf(log_error, "undefined Symbol %s", instruction_line->operand_data_arr[i].operand.name);
				LOG(INFO, PARAMS, "%s", log_error);
				if (SUCCESS != string_vector_push_back(assembler_data->error_founds, log_error))
				{
					LOG(ERROR, PARAMS, "Memory allocation failed in _build_rest_of_instruction_code");
					return FUNC_ERR;
				}
			}
			is_external = symbol->symbol_location == SYMBOL__EXTERN;
			if (is_external && instruction_line->L != -1)
			{
				extern_user = _init_extern_user(label_name, curr_IC);
				queue_enqueue(assembler_data->extern_symbols, extern_user);
			}
			else
			{
				/* validate that requested index is not out of range */
				if (instruction_line->operand_data_arr[i].operand.data_index.arr_index >= symbol->num_elements)
				{
					sprintf(log_error, "index %d is out of range for symbol %s, found in line : %d\n", instruction_line->operand_data_arr[i].operand.data_index.arr_index, instruction_line->operand_data_arr[i].operand.data_index.arr_name, assembler_data->line_count);
					PUSH_ERROR_AND_LOG(log_error, assembler_data->error_founds);
				}
			}
			word = set_bits_short(word, is_external ? ARE_EXTERNAL : ARE_RELOCATABLE, 2, 0);
			word = set_bits_short(word, symbol->symbol_value, 12, 2);
			assembler_data->machine_binary_code[curr_IC] = word;
			++curr_IC;
			assembler_data->machine_binary_code[curr_IC] = instruction_line->operand_data_arr[i].operand.data_index.arr_index << 2;;
			break;
		}
		case ADDRESSING__IMMEDIATE:
		case ADDRESSING__DIRECT_REGISTER:
		{
			break;
		}
		}
		++curr_IC;
	}

	return SUCCESS;
}

/* --------------------- external function definitions -------------------------*/
int assembler_second_pass(assembler_data_t *assembler_data, FILE *input_file)
{
	enum line_type line_type;
	char line[MAX_LINE_SIZE];
	instruction_line_t *instruction_line;

	assembler_data->line_count = 1;
	assembler_data->IC = IC_INITIAL_VALUE;


	/* go back to the beginning of the file */  
	if (SUCCESS != fseek(input_file, 0, SEEK_SET))
	{
		LOG(ERROR, PARAMS, "fseek failed");
		return FUNC_ERR;
	}
	memset(line, NEWLINE_CHARACTER, MAX_LINE_SIZE);

	while (fgets(line, MAX_LINE_SIZE, input_file) != NULL)
	{
		line[MAX_LINE_SIZE - 1] = NEWLINE_CHARACTER;
		line_type = identify_line_type(line); 

		if (line_type == LINE__INSTRUCTION)
		{
			instruction_line = (instruction_line_t *)queue_dequeue(assembler_data->parsed_instruction_lines);
			if (instruction_line == NULL)
			{
				LOG(ERROR, PARAMS, "failed to dequeue instruction line");
				return FUNC_ERR;
			}
			if (instruction_line->L == -1)
			{
				free(instruction_line);
				continue;
			}
			_build_rest_of_instruction_code(assembler_data, instruction_line);
			assembler_data->IC += instruction_line->L;
			
			/* no need anymore for data inside instruction line */
			free(instruction_line);
		}
		++assembler_data->line_count;
		memset(line, NEWLINE_CHARACTER, MAX_LINE_SIZE);

	}
    hash_table_for_each(assembler_data->symbol_table, _validate_entry_value, assembler_data);

	if (!queue_is_empty(assembler_data->parsed_instruction_lines))
	{
		LOG(ERROR, PARAMS, "parsed_instruction_lines is not empty even though we finished reading the file");
		return FUNC_ERR;
	}

	return SUCCESS;
}
