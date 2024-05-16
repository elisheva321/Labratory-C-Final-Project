/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  assembler_helper.h
 * @brief  This file contains the declarations of the helper
 * functions functions and data structures used in the assembler.
 *
 *********************************************************************/

#ifndef ASSEMBLER_HELPER_H
#define ASSEMBLER_HELPER_H

#include "../hash_table/hash_table.h"
#include "../string_vector/string_vector.h"
#include "../assembler/assembler_consts.h"
#include "../dynamic_queue/dynamic_queue.h"
#include "../general_const/general_const.h"

/* forward declaration for assembler_data_t*/
struct assembler_data;
struct instruction_line;
typedef int (*line_handler)(const char *line, struct assembler_data *assembler_data, short found_symbol,HashTable *macro_table);

/*================================== lines type============================*/
typedef enum line_type
{
	LINE__EMPTY,
	LINE__COMMENT,
	LINE__DIRECTIVE,
	LINE__INSTRUCTION,
	LINE__CONST_DEFINE,
	MAX_LINE_TYPE

} line_type_t;

/*======================================================================*/

typedef struct assembler_data
{
	short IC; /* (instruction counter) */
	short DC; /* data counter */

	short machine_binary_code[MEMORY_SIZE]; /* array for the generated machine code*/
	short data_memory[MEMORY_SIZE]; /* array for the data allocated in the given file */
	HashTable *symbol_table; /* hash table for the symbols */
	HashTable *action_into_binary_map; /* hash table for the action into binary map. key: action str. value: action code*/
	StringVector *error_founds; /* vector for the errors found in the given file */
	StringVector *warning_founds; /* vector for the warnings found in the given file */
	short line_count; /* line counter */ 
	line_handler line_handlers_arr[MAX_LINE_TYPE]; /* array of function pointers for handling different types of lines */
	Queue *parsed_instruction_lines; /* queue for the parsed instruction lines */
	Queue *extern_symbols; /* queue for the extern symbols */
	int has_entry; /* True if we found at least one entry label in the file*/
	int *instruction_code_arr; /* array for the instructions code */
} assembler_data_t;

/**
 * Calculates the number of words required for an instruction code.
 *
 * This function takes a pointer to an instruction_line struct and calculates the number of words
 * required to represent the instruction code. The calculation is based on the specific instruction
 * and its operands.
 *
 * @param instruction_line A pointer to the instruction_line struct representing the instruction.
 * @return The number of words required for the instruction code.
 */
short calc_num_word_for_instruction_code(struct instruction_line *instruction_line);


/**
 * Initializes the line handlers array.
 *
 * This function initializes the line handlers array in the given assembler_data structure.
 * The line handlers array is used to store function pointers for handling different types of assembly lines (comment, empty, instruction, data, const define).
 *
 * @param assembler_data A pointer to the assembler_data_t structure.
 */
void init_line_handlers_arr(assembler_data_t *assembler_data);


/**
 * Checks if the given line starts with a symbol.
 *
 * @param line The line to be checked.
 * @return 1 if the line is a symbol, 0 otherwise.
 */
int is_symbol(const char *line);


/**
 * Identifies the type of the given line.
 *
 * This function takes a string representing a line of code and determines its type.
 * The line can be an instruction, a label, a comment, or an empty line.
 *
 * @param line The line of code to be identified.
 * @return The type of the line: 0 for instruction, 1 for label, 2 for comment, or -1 for empty line.
 */
int identify_line_type(char *line);

/* ================================== symbol ===================== */

typedef struct extern_user {
	char label_name[MAX_LABEL_SIZE];
	short address;
} extern_user_t;

enum symbol_location
{
	SYMBOL__ENTRY,
	SYMBOL__EXTERN
};

enum memory_type
{
	MEM_TYPE__INSTRUCTION,
	MEM_TYPE__DATA,
	MEM_TYPE__DEFINE
};

typedef struct symbol
{
	char symbol_name[MAX_LABEL_SIZE];
	short symbol_location;
	short memory_type;
/*  The size of num_elements is 6 bits.
 *  This limitation is due to the maximum array size of ~40,
 *  which is the maximum line size. */
 	short num_elements;
/* value of an address or const defined, which is max 12 bits. */
	short symbol_value;
} symbol_t;

/* ================================== directive_line ===================== */
enum directive_type
{
	/* data means that we need to allocate memory in the data image */
	DIR__DATA, /* example: ".data 7, -57, +17, 9", "XYZ: .data 7, –57, +17, 9" */
	/* only one paramater, valid string */
	DIR__STRING, /* example: STR: .string "abcd" */
	/* should not have label at the beginning  (we can print warning)*/
	/* tells the assembly other files can access the label */
	DIR__ENTRY,
	/* should not have label at the beginning */
	/* let the assembler know that the label definition is in a different file.*/
	DIR__EXTERN
};

typedef struct directive_line
{
	char label[MAX_LABEL_SIZE];
	int directive_type;
	char *data_ptr; /* pointer to the actual data */
	short num_elements;
} directive_line_t;

/* ================================== instruction_line ===================== */
/* This two-dimensional array defines the valid addressing types for operands
 * in each instruction . 
 * A value of 1 indicates that the corresponding addressing type is valid, while a value
 * of 0 indicates that it is invalid.
 * The first four elements in each row represent the valid addressing types for the first operand,
 * and the last four elements represent the valid addressing types for the second operand. 
 */
extern const short valid_addressing_types_for_instructions[NUM_INSTRUCTIONS][MAX_POSSIBLE_ADDRESSING_TYPE_FOR_TWO_OPERANDS];

/* This array represents the number of operands for each instruction, indexed by the instruction code.
 * Each element in the array corresponds to a specific instruction code (0-15), indicating the number of operands that instruction expects.
*/
extern const short num_operands_for_instructions[NUM_INSTRUCTIONS];


/* possible addressing type for operand defined in instruction line*/
enum addressing_type 
{
	ADDRESSING__IMMEDIATE,
	ADDRESSING__DIRECT,
	ADDRESSING__DATA_INDEX,
	ADDRESSING__DIRECT_REGISTER
};

/* possible types for operand defined in instruction line*/
typedef enum
{
	OPERAND_TYPE__NUMBER, /* for immediate addressing/registers */
	OPERAND_TYPE__NAME,	  /* for symbol */
	OPERAND_TYPE__DATA_INDEX, /*for array */
	OPERAND_TYPE__NO_OPERAND /* when there is operand in the instruction*/
} operand_type;

/* represents operand of type data index defined in instruction line*/
typedef struct data_index
{
	char arr_name[MAX_LABEL_SIZE];
	short arr_index;
} data_index_t;

/* represents operand value defined in instruction line */
typedef union operand
{
	short number;
	char name[MAX_LABEL_SIZE];
	data_index_t data_index;
} operand_t;

/* represents all data needed for operand defined in instruction line */
typedef struct operand_data
{
	operand_type type;
	operand_t operand;
	enum addressing_type addressing_type;
} operand_data_t;

/* represents parsed data of instruction line */
typedef struct instruction_line
{
	char label[MAX_LABEL_SIZE]; /* not always exists*/
	char instruction_name[INSTRUCTION_LENGTH + 1]; /* for example: mov, sub ..*/
	short instruction_code; /* the code of the instruction  for example for mov: 0*/
	operand_data_t operand_data_arr[MAX_NUM_OPERAND]; /* array of operands */
	short L; /* num of words needed for the instruction line*/
} instruction_line_t;

/* ================================== const_define_line ===================== */
/* format: define const_name = const_value_integer
	example .define len = 4
			.define init = -3; */
/* everywhere in code where there is the const_define, it needs to be replaced with the integer value. */
typedef struct const_define_line
{
	char const_name[MAX_LABEL_SIZE];
	int const_value;
} const_define_line_t;

/*============================functions declarations========================================*/
/* handlers for different types of lines functions declarations */
int handle_comment_or_empty_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table);
int handle_directive_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table);
int handle_instruction_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table);
int handle_const_defined_line(const char *line, assembler_data_t *assembler_data, short found_symbol, HashTable *macro_table);

/* line type identification functions declarations */
int identify_comment_line(char *line);
int identify_empty_line(char *line);
int identify_const_define_line(char *line);
int identify_directive_line(char *line);
int identify_instruction_line(char *line);

/* instruction line helper functions declarations */
int parse_instruction_line(const char *line, assembler_data_t *assembler_data, instruction_line_t **instruction_line, HashTable *macro_table);
short get_IC_value(assembler_data_t *assembler_data);
short calc_num_word_for_instruction_code(instruction_line_t *instruction_line);
short build_first_word(instruction_line_t *instruction_line);
short _get_sum_memory(assembler_data_t *assembler_data);
void set_first_word(assembler_data_t *assembler_data, short i_mem_code, short instruction_code_first_word);
short build_word_and_set_in_memory(assembler_data_t *assembler_data, instruction_line_t *instruction_line, short i_mem_code, short num_operand);
int allocate_mem_for_instruction_code(assembler_data_t *assembler_data, short L);

/* handlers for different num of operand in instructions line */
int zero_operands_instruction(char *line, instruction_line_t **instruction_line, assembler_data_t *assembler_data);
int one_operand_instruction(char *line, instruction_line_t **instruction_line, assembler_data_t *assembler_data);
int two_operands_instruction(char *line, instruction_line_t **instruction_line, assembler_data_t *assembler_data);

/* set addressing type data functions declarations */
int set_operand_immediate_addressing(char *operand_name, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data);
int set_operand_direct_addressing(char *operand_name, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data);
int set_operand_define_index_addressing(char *operand_name, char *open_pos, char *close_pos, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data);
int set_operand_register_addressing(char *operand_name, int register_index, int operand_number, instruction_line_t **instruction_line, assembler_data_t *assembler_data);

int identify_operand_define_index_addressing(char *operand_name, char **open_pos , char **close_pos);
int identify_operand_register_addressing(char *operand_name, int *register_index);

int is_operand_addressing_valid(int addressing_type, short instruction_code, int operand_number);

/* directive line helper functions declarations */
int find_directive_line_type_from_begin(const char *line);
int find_directive_line_type(const char *line);
int find_instruction(const char *line);

int parse_and_set_directive_line(const char *line, assembler_data_t *assembler_data, HashTable *macro_table, directive_line_t **directive_line);

int allocate_memory_and_set_data_for_str(assembler_data_t *assembler_data, directive_line_t *directive_line, const char *line);
short allocate_mem_for_data(assembler_data_t *assembler_data, directive_line_t *directive_line, int num_elements);

int data_array_validation_and_set(char *line, assembler_data_t *assembler_data, int curr_DC);
void deal_with_more_than_two_quotation_marks(char *data_content, int *i, int *data_content_size);

int set_directive_type_data(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line);
int set_directive_type_string(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line);
int set_directive_type_entry(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line);
int set_directive_type_extern(char *line, char *directive_name, assembler_data_t *assembler_data, directive_line_t **directive_line);

/* const define handler */
int parse_const_define_line(const char *line, HashTable *macro_table, assembler_data_t *assembler_data, const_define_line_t **const_define_line);

/* symbol handler */
void init_symbol_data(symbol_t *symbol, const char *label_name, enum memory_type memory_type, short symbol_value, short num_elements);
void init_symbol_location_data(symbol_t *symbol, const char *label_name, enum symbol_location symbol_location);
void update_symbol_data(symbol_t *symbol, const char *label_name, enum memory_type memory_type, short symbol_value, short num_elements);
void update_symbol_location_data(symbol_t *symbol, enum symbol_location symbol_location);
int init_and_add_symbol_table(assembler_data_t *assembler_data, const char *label_name, enum memory_type memory_type, short symbol_value, short num_elements);
int init_and_add_location_to_symbol_table(assembler_data_t *assembler_data, const char *label_name, enum symbol_location symbol_location);
void add_value_to_data_symbol(const char *key, void *data, void *IC);
void add_value_to_all_data_symbol(assembler_data_t *assembler_data);
int find_symbol_value(int *found_value , char *lookup_name, HashTable *symbol_table);

/*label_handler*/
int handle_label(char **line, char *label_name , int line_type, HashTable *macro_table, assembler_data_t *assembler_data);

/* validation functions*/
int validate_label_name(char *label_name, int *label_size, int line_type, HashTable *macro_table,  assembler_data_t *assembler_data);
int validate_last_char_label_name(char *label_name, int *label_size, int line_count, StringVector *error_founds);
int validate_first_char_label_name(char *label_name, int line_count, StringVector *error_founds);
int validate_label_name_characters(char *label_name,  int line_count, StringVector *error_founds);
int validate_is_label_name_register(char *label_name, int line_count, StringVector *error_founds);
int validate_is_label_name_instruction(char *label_name, int line_count, StringVector *error_founds);
int validate_is_label_name_const_define(char *label_name, HashTable *symbol_table, int line_count, StringVector *error_founds);
int validate_is_label_name_dupe(char *label_name, int line_type, HashTable *symbol_table, int line_count, StringVector *error_founds);
int validate_is_label_name_macro(char *label_name, HashTable *macro_table, int line_count, StringVector *error_founds);
int validate_is_label_name_directive(char *label_name, int line_count, StringVector *error_founds);
int validate_is_label_name_macro_defenition(char *label_name, int line_count, StringVector *error_founds);
int validate_extern_label(char *label_name, assembler_data_t *assembler_data);

int validate_is_label_name_entry_and_extern(char *label_name, directive_line_t *directive_line, assembler_data_t *assembler_data);
int validate_label_for_direct_addressing(char *operand_name);

#endif /* ASSEMBLER_HELPER_H */
