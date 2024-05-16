/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  instruction_handler.c
 * @brief This file contains the implementation of the instruction line handler functions.
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


const short valid_addressing_types_for_instructions[NUM_INSTRUCTIONS][MAX_POSSIBLE_ADDRESSING_TYPE_FOR_TWO_OPERANDS] = {
	{1, 1, 1, 1, 0, 1, 1, 1}, /* 0  mov */
	{1, 1, 1, 1, 1, 1, 1, 1}, /* 1  cmp */
	{1, 1, 1, 1, 0, 1, 1, 1}, /* 2  add */
	{1, 1, 1, 1, 0, 1, 1, 1}, /* 3  sub */
	{0, 0, 0, 0, 0, 1, 1, 1}, /* 4  not */
	{0, 0, 0, 0, 0, 1, 1, 1}, /* 5  clr */
	{0, 1, 1, 0, 0, 1, 1, 1}, /* 6  lea */
	{0, 0, 0, 0, 0, 1, 1, 1}, /* 7  inc */
	{0, 0, 0, 0, 0, 1, 1, 1}, /* 8  dec */
	{0, 0, 0, 0, 0, 1, 0, 1}, /* 9  jmp */
	{0, 0, 0, 0, 0, 1, 0, 1}, /* 10 bne */
	{0, 0, 0, 0, 0, 1, 1, 1}, /* 11 red */
	{0, 0, 0, 0, 1, 1, 1, 1}, /* 12 prn */
	{0, 0, 0, 0, 0, 1, 0, 1}, /* 13 jsr */
	{0, 0, 0, 0, 0, 0, 0, 0}, /* 14 rts */
	{0, 0, 0, 0, 0, 0, 0, 0}, /* 15 hlt */
};

const short num_operands_for_instructions[NUM_INSTRUCTIONS] = {2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0};

int parse_instruction_line(const char *line, assembler_data_t *assembler_data, instruction_line_t **instruction_line, HashTable *macro_table)
{
	char instruction_name_local[INSTRUCTION_LENGTH + 1];
	char label_name[MAX_LABEL_SIZE];
	int num_of_operands;
	int *instruction_code_ptr = NULL;
	char *line_runner = (char *)line;
	char log_error[MAX_LINE_SIZE];
	int ret_val;
	char *label_postfix_ptr = strchr(line, LABEL_POSTFIX);

	*instruction_line = (instruction_line_t *)malloc(sizeof(instruction_line_t));
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(*instruction_line, "Memory allocation for instruction_line failed", FUNC_ERR);

	if (skip_whitespace(&line_runner, assembler_data->line_count , assembler_data->error_founds)) /* skipping whitespaces between words*/
	{
		sprintf(log_error, "no operands after instruction. found in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	/* in case the line contains a label*/
	if( label_postfix_ptr != NULL)
	{
		if( memchr(line, NEWLINE_CHARACTER, label_postfix_ptr - line) == NULL )/* validating that label_postfix_ptr is before the newline character*/
		{
			ret_val = handle_label(&line_runner, label_name, LINE__INSTRUCTION, macro_table, assembler_data);
			if ( ret_val != SUCCESS)
			{
					return ret_val;
			}
			strcpy((*instruction_line)->label, label_name);
		}
	}

	/* finding instruction code*/
	strncpy(instruction_name_local, line_runner, INSTRUCTION_LENGTH);
	instruction_name_local[INSTRUCTION_LENGTH] = NULL_TERMINATOR;
	instruction_code_ptr = (int *)hash_table_lookup(assembler_data->action_into_binary_map, instruction_name_local);
	if (instruction_code_ptr == NULL)
	{
		if( find_instruction(instruction_name_local) == FOUND)
		{
			LOG(ERROR, PARAMS, "hash_table_lookup function failed at process_instruction_line\n");
			return FUNC_ERR;
		}
		else
		{
			sprintf(log_error, "invalid label name, label doesn't end with ':'. found in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
	}
	(*instruction_line)->instruction_code = *instruction_code_ptr;
	strcpy((*instruction_line)->instruction_name, instruction_name_local);

	num_of_operands = num_operands_for_instructions[(*instruction_line)->instruction_code];

	if (num_of_operands == 0)
	{
		return zero_operands_instruction(line_runner + INSTRUCTION_LENGTH, instruction_line, assembler_data);
	}

	if (num_of_operands == 1)
	{
		return one_operand_instruction(line_runner + INSTRUCTION_LENGTH, instruction_line, assembler_data);
	}

	if (num_of_operands == 2)
	{
		return two_operands_instruction(line_runner + INSTRUCTION_LENGTH, instruction_line, assembler_data);
	}

	return SUCCESS;
}

short get_IC_value(assembler_data_t *assembler_data)
{
	return assembler_data->IC;
}

short calc_num_word_for_instruction_code(instruction_line_t *instruction_line)
{
	short i;
	short L = 1; /* each instruction code has at least one word*/
	short num_operands = 0;

	for (i = 0; i < MAX_NUM_OPERAND; ++i)
	{
		if (instruction_line->operand_data_arr[i].type != OPERAND_TYPE__NO_OPERAND)
		{
			num_operands++;
			/* Increment L based on operand type, addressing for specific optimization*/
			switch (instruction_line->operand_data_arr[i].addressing_type)
			{
			case ADDRESSING__IMMEDIATE:
			case ADDRESSING__DIRECT:
			{
				L++;
				break;
			}
			case ADDRESSING__DIRECT_REGISTER:
			{
				L++;
				break;
			}
			case ADDRESSING__DATA_INDEX:
			{
				/* needs 2, one for the address of the arr, one for the index */
				L += 2;
				break;
			}
			}
		}
	}
	/* Specific scenario optimization: both operands use direct register addressing
	Assuming a specific case where L should be adjusted if both operands meet certain conditions */
	if (num_operands == MAX_NUM_OPERAND &&
		instruction_line->operand_data_arr[0].addressing_type == ADDRESSING__DIRECT_REGISTER &&
		instruction_line->operand_data_arr[1].addressing_type == ADDRESSING__DIRECT_REGISTER)
	{
		L -= 1;
	}
	return L;
}

short build_first_word(instruction_line_t *instruction_line)
{
	static const short NUM_BITS_ADDRESSING_TYPE = 2;
	static const short NUM_BITS_INSTRUCTION_CODE = 4;

	static const short STARTING_BIT_SRC_OPERAND = 4;
	static const short STARTING_BIT_DEST_OPERAND = 2;
	static const short STARTING_BIT_INSTRUCTION_CODE = 6;

	short word = 0;
	short start_bit_for_first_operand = STARTING_BIT_DEST_OPERAND;

	if (instruction_line->operand_data_arr[1].type != OPERAND_TYPE__NO_OPERAND)
	{
		start_bit_for_first_operand = STARTING_BIT_SRC_OPERAND;
		word = set_bits_short(word, instruction_line->operand_data_arr[1].addressing_type, NUM_BITS_ADDRESSING_TYPE, STARTING_BIT_DEST_OPERAND);
	}

	if (instruction_line->operand_data_arr[0].type != OPERAND_TYPE__NO_OPERAND)
	{
		word = set_bits_short(word, instruction_line->operand_data_arr[0].addressing_type, NUM_BITS_ADDRESSING_TYPE, start_bit_for_first_operand);
	}

	word = set_bits_short(word, instruction_line->instruction_code, NUM_BITS_INSTRUCTION_CODE, STARTING_BIT_INSTRUCTION_CODE);

	return word;
}

short _get_sum_memory(assembler_data_t *assembler_data)
{
	return assembler_data->IC + assembler_data->DC;
}

void set_first_word(assembler_data_t *assembler_data, short i_mem_code, short instruction_code_first_word)
{
	assembler_data->machine_binary_code[i_mem_code] = instruction_code_first_word;
}

short build_word_and_set_in_memory(assembler_data_t *assembler_data, instruction_line_t *instruction_line, short i_mem_code, short num_operand)
{
	static const int NUM_BITS_FOR_REGISTER = 3;
	static const int STARTING_BIT_FOR_SRC_REGISTER = 5;
	static const int STARTING_BIT_FOR_DEST_REGISTER = 2;

	short word = 0;
	short addressing_type = instruction_line->operand_data_arr[num_operand].addressing_type;
	short word_address = i_mem_code;
	static short is_first_operand_register = FALSE;
	int starting_bit_for_register = 0;

	switch (addressing_type)
	{
	case ADDRESSING__IMMEDIATE:
	{
		/* convert number into twos_complement representation */
		word = instruction_line->operand_data_arr[num_operand].operand.number;
		/* moving 2 bits to the left cause first 2 bits belong to ARE type */
		word <<= 2;		
		break;
	}
	case ADDRESSING__DIRECT_REGISTER:
	{
		if (num_operand == 0)
		{
			is_first_operand_register = TRUE;
			assembler_data->machine_binary_code[(word_address)] = 0;
		}
		else {
			if (is_first_operand_register)
			{
				word_address -= 1;
				is_first_operand_register = FALSE;
			}
		}
		word = assembler_data->machine_binary_code[(word_address)];
		starting_bit_for_register = STARTING_BIT_FOR_SRC_REGISTER - num_operand * NUM_BITS_FOR_REGISTER;
		if (instruction_line->operand_data_arr[1].type == OPERAND_TYPE__NO_OPERAND)
		{
			/* handle case where only one register operand in the command, in that case this operand will be dest and not src operand*/
			starting_bit_for_register = STARTING_BIT_FOR_DEST_REGISTER;
		}
		word = set_bits_short(word, instruction_line->operand_data_arr[num_operand].operand.number, NUM_BITS_FOR_REGISTER, starting_bit_for_register);
		break;
	}
	case ADDRESSING__DATA_INDEX:
	{
		word = instruction_line->operand_data_arr[num_operand].operand.data_index.arr_index;
		/* moving 2 bits to the left cause first 2 bits belong to ARE type */
		word <<= 2;
		word_address += 1;
	}
	}
	assembler_data->machine_binary_code[word_address] = word;

	if (num_operand == 1 || addressing_type != ADDRESSING__DIRECT_REGISTER)
	{
		/* unset value of this flag for next instruction line */
		is_first_operand_register = FALSE;
	}
	return word_address;
}

int allocate_mem_for_instruction_code(assembler_data_t *assembler_data, short L)
{
	if (_get_sum_memory(assembler_data) + L >= MEMORY_SIZE)
	{
		return ERR;
	}
	assembler_data->IC += L;
	return SUCCESS;
}

int zero_operands_instruction(char *line, instruction_line_t **instruction_line, assembler_data_t *assembler_data)
{
	int i = 0;
	char log_error[MAX_LINE_SIZE];

	for (; line[i] != NEWLINE_CHARACTER; i++) /*validating that there are no non whitespaces characters after the instruction */
	{
		if (!isspace(line[i]))
		{
			sprintf(log_error, "there are non whitespaces characters after the instruction defenition in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
	}
	(*instruction_line)->operand_data_arr[0].type = OPERAND_TYPE__NO_OPERAND;
	(*instruction_line)->operand_data_arr[1].type = OPERAND_TYPE__NO_OPERAND;
	return SUCCESS;
}

int one_operand_instruction(char *line, instruction_line_t **instruction_line, assembler_data_t *assembler_data)
{
	char operand_name[MAX_LINE_SIZE];
	int operand_size = 0;
	char log_error[MAX_LINE_SIZE];
	int ret_val = SUCCESS;
	char *open_pos;
	char *close_pos;
	int i = 0;
	int register_index = 0;

	if (skip_whitespace(&line, assembler_data->line_count , assembler_data->error_founds) == ERR) /*skipping whitespaces between words*/
	{
		(*instruction_line)->operand_data_arr[0].type = OPERAND_TYPE__NO_OPERAND;
		(*instruction_line)->operand_data_arr[1].type = OPERAND_TYPE__NO_OPERAND;
		sprintf(log_error, "missing operand name in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	strcpy(operand_name, line + i);
	for (; !isspace(line[i]); i++)
	{
		operand_size++;
	}
	operand_name[operand_size] = NULL_TERMINATOR;
	(*instruction_line)->operand_data_arr[1].type = OPERAND_TYPE__NO_OPERAND;

	for( i= operand_size ; line[i]!= NEWLINE_CHARACTER; ++i)
	{
		if( !isspace(line[i]))
		{
			sprintf(log_error, "too many operands for instruction in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
	}

	/*addressing type 0 - immediate_addressing*/
	if (operand_name[0] == INTEGER_OPERAND_PREFIX)
	{
		return set_operand_immediate_addressing(operand_name, 0, instruction_line, assembler_data);
	}

	/*addressing type 2 - define_index_addressing,*/
	else if (identify_operand_define_index_addressing(operand_name, &open_pos, &close_pos))
	{
		return  set_operand_define_index_addressing(operand_name, open_pos, close_pos, 0, instruction_line, assembler_data);
	}

	/*addressing type 3 - register_addressing*/
	else if ( identify_operand_register_addressing(operand_name, &register_index))
	{
		return set_operand_register_addressing(operand_name, register_index,  0, instruction_line, assembler_data);
	}

	/*addressing type 1 - direct_addressing*/
	else if (validate_label_for_direct_addressing(operand_name) != ERR)
	{
		return  set_operand_direct_addressing(operand_name, 0, instruction_line, assembler_data);
	}

	sprintf(log_error, "Syntax error at line %d: No matching addressing type.", assembler_data->line_count);
	PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	return ret_val;
}

int two_operands_instruction(char *line, instruction_line_t **instruction_line, assembler_data_t *assembler_data)
{
	char operand_name[MAX_LINE_SIZE];
	int operand_size = 0;
	int operand_number = 0;
	int i = 0;
	int addressing_type_found = 1;
	char *open_pos;
	char *close_pos;
	char log_error[MAX_LINE_SIZE];
	int register_index = 0;

	for (operand_number = 0; operand_number < MAX_NUM_OPERAND; ++operand_number)
	{
		if (operand_number == 1) /*reseting*/
		{
			operand_size = 0;
			addressing_type_found = 1;
			i = 0;
		}

		if (skip_whitespace(&line, assembler_data->line_count , assembler_data->error_founds) == ERR) /*skipping whitespaces between words*/
		{
			if(operand_number == 0)
			{
				(*instruction_line)->operand_data_arr[0].type = OPERAND_TYPE__NO_OPERAND;
			}
			(*instruction_line)->operand_data_arr[1].type = OPERAND_TYPE__NO_OPERAND;
			sprintf(log_error, "missing operand name in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}

		strcpy(operand_name, line + i);
		for (; !isspace(line[i]) && line[i] != COMMA && line[i] !=  NEWLINE_CHARACTER; i++)
		{
			operand_size++;
		}

		line += operand_size;

		if(operand_number == 0)
		{
			if (skip_whitespace(&line, assembler_data->line_count , assembler_data->error_founds) == ERR) /*skipping whitespaces between words*/
			{
				sprintf(log_error, "missing operand name in line: %d", assembler_data->line_count);
				PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
			}
		}
		else
		{
			while(isspace(*line) && (*line) !=  NEWLINE_CHARACTER)
			{
				++line;
			}
			if((*line) != NEWLINE_CHARACTER )
			{
				sprintf(log_error, "too many operands for instruction. found in line: %d", assembler_data->line_count);
				PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
			}
			
		}

		if (line[0] == COMMA && operand_number == 0)
		{
			++line;
		}
		else if(line[i] != COMMA && operand_number == 0 && line[i] != NEWLINE_CHARACTER)
		{
			sprintf(log_error, "missing a comma between two operands in line: %d", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
		operand_name[operand_size] = NULL_TERMINATOR;

		/*addressing type 0 - immediate_addressing*/
		if (operand_name[0] == INTEGER_OPERAND_PREFIX)
		{
			addressing_type_found = set_operand_immediate_addressing(operand_name, operand_number, instruction_line, assembler_data);
			if( addressing_type_found != SUCCESS)
			{
				return addressing_type_found;
			}
		}

		/*addressing type 2 - define_index_addressing,*/
		else if ((addressing_type_found != SUCCESS) && identify_operand_define_index_addressing(operand_name, &open_pos, &close_pos))
		{
			addressing_type_found = set_operand_define_index_addressing(operand_name, open_pos, close_pos, operand_number, instruction_line, assembler_data);
			if( addressing_type_found != SUCCESS)
			{
				return addressing_type_found;
			}
		}

		/* addressing type 3 - register_addressing*/
		else if ((addressing_type_found != SUCCESS) && identify_operand_register_addressing(operand_name, &register_index))
		{
			addressing_type_found = set_operand_register_addressing(operand_name, register_index, operand_number, instruction_line, assembler_data);
			if( addressing_type_found != SUCCESS)
			{
				return addressing_type_found;
			}
		}

		/* addressing type 1 - direct_addressing*/
		else if ((addressing_type_found!= SUCCESS) && (validate_label_for_direct_addressing(operand_name) != ERR))
		{
			addressing_type_found = set_operand_direct_addressing(operand_name, operand_number, instruction_line, assembler_data);
			if( addressing_type_found != SUCCESS)
			{
				return addressing_type_found;
			}
		}

		/* in case of no matching addressing type*/
		else if (addressing_type_found != SUCCESS)
		{
			sprintf(log_error, "Syntax error at line %d: No matching adressing type.", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
	}

	return SUCCESS;
}

int identify_operand_register_addressing(char *operand_name, int *register_index)
{
	for ((*register_index) = 0; (*register_index) < NUM_OF_REGISTERS; ++(*register_index))
	{
		if (strcmp(operand_name, REGISTERS_NAMES[(*register_index)]) == 0)
		{

			return FOUND;
		}
	}
		
	return NOT_FOUND;
}

int set_operand_register_addressing(char *operand_name, int register_index, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data)
{
	char log_error[MAX_LINE_SIZE];
	char operand_register[REGISTER_LENGTH + 1];
	
	strcpy(operand_register, REGISTERS_NAMES[register_index]);
	(*instruction_line)->operand_data_arr[operand_number].type = OPERAND_TYPE__NUMBER;
	(*instruction_line)->operand_data_arr[operand_number].addressing_type = ADDRESSING__DIRECT_REGISTER;
	(*instruction_line)->operand_data_arr[operand_number].operand.number = register_index;
	if (is_operand_addressing_valid(ADDRESSING__DIRECT_REGISTER, (*instruction_line)->instruction_code, operand_number) == ERR)
	{
		sprintf(log_error, "Invalid addressing type %d for instruction. found in line: %d", ADDRESSING__DIRECT_REGISTER, assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}
	return SUCCESS;
}

int identify_operand_define_index_addressing(char *operand_name, char **open_pos , char **close_pos)
{
	*open_pos = strchr(operand_name, '[');
	*close_pos = strchr(operand_name, ']');
	if ((*open_pos) != NULL && (*close_pos) != NULL)
	{
		return FOUND;
	}
	return NOT_FOUND;
}

int set_operand_define_index_addressing(char *operand_name, char *open_pos, char *close_pos, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data)
{
	int index = 0;
	char index_name[MAX_LABEL_SIZE + 1];
	int index_size = 0;
	char array_name[MAX_LABEL_SIZE + 1];
	int array_name_size = 0;
	char log_error[MAX_LINE_SIZE];

	if (open_pos > close_pos)
	{
		sprintf(log_error, "Invalid order of '[' and ']' in the operand name, found in line %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	array_name_size = open_pos - operand_name;
	strncpy(array_name, operand_name, array_name_size);
	array_name[array_name_size] = NULL_TERMINATOR;

	index_size = close_pos - open_pos - 1;
	strncpy(index_name, operand_name + array_name_size + 1, index_size);
	index_name[index_size] = NULL_TERMINATOR;

	if (!isdigit(index_name[0]) && index_name[0] != '-') /*in case the index is const define name*/
	{
		if( find_symbol_value(&index , index_name, assembler_data->symbol_table) == ERR)
		{
			sprintf(log_error, "invalid const name: %s. found in line: %d", operand_name, assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
	}
	else if(index_name[0] == '-')
	{
		sprintf(log_error, "index %d is out of range. found in line: %d", atoi(index_name), assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}
	else
	{
		index = atoi(index_name);
	}

	(*instruction_line)->operand_data_arr[operand_number].type = OPERAND_TYPE__DATA_INDEX;
	(*instruction_line)->operand_data_arr[operand_number].addressing_type = ADDRESSING__DATA_INDEX;
	strcpy((*instruction_line)->operand_data_arr[operand_number].operand.data_index.arr_name, array_name);
	(*instruction_line)->operand_data_arr[operand_number].operand.data_index.arr_index = index;

	if (is_operand_addressing_valid(ADDRESSING__DATA_INDEX, (*instruction_line)->instruction_code, operand_number) == ERR)
	{
		sprintf(log_error, "Invalid addressing type %d for instruction. found in line: %d", ADDRESSING__DATA_INDEX, assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	return SUCCESS;
}


int set_operand_direct_addressing(char *operand_name, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data)
{
	char log_error[MAX_LINE_SIZE];
	(*instruction_line)->operand_data_arr[operand_number].type = OPERAND_TYPE__NAME;
	(*instruction_line)->operand_data_arr[operand_number].addressing_type = ADDRESSING__DIRECT;
	strcpy((*instruction_line)->operand_data_arr[operand_number].operand.name, operand_name);

	if (is_operand_addressing_valid(ADDRESSING__DIRECT, (*instruction_line)->instruction_code, operand_number) == ERR)
	{
		sprintf(log_error, "Invalid addressing type %d for instruction. found in line: %d", ADDRESSING__DIRECT, assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	return SUCCESS;
}

int set_operand_immediate_addressing(char *operand_name, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data)
{
	int i = 0;
	int number_val = 0;
	char log_error[MAX_LINE_SIZE];

	(*instruction_line)->operand_data_arr[operand_number].type = OPERAND_TYPE__NUMBER;
	(*instruction_line)->operand_data_arr[operand_number].addressing_type = ADDRESSING__IMMEDIATE;

	operand_name += 1; /* skipping the char # to extract the integer*/

	if( isspace(operand_name[0]))
	{
		sprintf(log_error, "whitespaces found between '#' and const define value. in line: %d", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}
	if (!isdigit(operand_name[0]) && operand_name[0] != '-') /* in case the number is const define name */
	{
		if( find_symbol_value(&number_val , operand_name, assembler_data->symbol_table) == ERR)
		{
			sprintf(log_error, "invalid const name: %s. found in line: %d", operand_name, assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}
	}

	else
	{
		if (operand_name[0] == '-')
		{
			i++;
		}

		if (strlen(operand_name) > MAX_INTEGER_LENGTH)
		{
			sprintf(log_error, "Integer value at line %d exceeds memory capacity", assembler_data->line_count);
			PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
		}

		for (; operand_name[i] != NULL_TERMINATOR; i++)
		{
			if (!isdigit(operand_name[i]))
			{
				sprintf(log_error, "Syntax error at line %d: Non-numeric characters found after # symbol.", assembler_data->line_count);
				PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
			}
		}

		number_val = atoi(operand_name);
	}

	if (number_val > MAX_INTEGER_VALUE || number_val < MIN_INTEGER_VALUE)
	{
		sprintf(log_error, "Integer value at line %d exceeds memory capacity", assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, ERR, assembler_data->error_founds);
	}

	(*instruction_line)->operand_data_arr[operand_number].operand.number = number_val;

	if (is_operand_addressing_valid(ADDRESSING__IMMEDIATE, (*instruction_line)->instruction_code, operand_number) == ERR)
	{
		sprintf(log_error, "Invalid addressing type %d for instruction. found in line: %d", ADDRESSING__IMMEDIATE, assembler_data->line_count);
		PUSH_ERROR_AND_RETURN( log_error, USER_ERR, assembler_data->error_founds);
	}

	return SUCCESS;
}


int is_operand_addressing_valid(int addressing_type, short instruction_code, int operand_number)
{
	if (operand_number == 1)
	{
		addressing_type += 4;
	}

	if (valid_addressing_types_for_instructions[instruction_code][addressing_type])
	{
		return SUCCESS;
	}
	return ERROR;
}	

int find_instruction(const char *line)
{
	int i = 0;
	 char *instruction_ptr;

	for (i = 0; i < NUM_INSTRUCTIONS; ++i)
	{
		instruction_ptr = strstr(line,INSTRUCTIONS_NAMES[i]);
		if (instruction_ptr!= NULL)
		{
			if(memchr(line, NEWLINE_CHARACTER, instruction_ptr - line) == NULL )/*validating that instruction_ptr is before the newline character*/
			{
				return FOUND;
			}	
		}
	}
	return NOT_FOUND;
}

