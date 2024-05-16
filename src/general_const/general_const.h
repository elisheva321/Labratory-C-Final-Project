/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file general_const.h
 * @brief This file contains all common consts for all the projects
 * 
 *********************************************************************/
#ifndef GENERAL_CONST_H
#define GENERAL_CONST_H

/* This file contains all common consts for all the projects*/
#define LOG_SEPARATOR ("========================================")

#define FALSE (0)
#define TRUE (1)

#define MAX_LABEL_SIZE (32)/*31 chars plus null terminator*/
#define NUM_OF_REGISTERS (8)
#define MEMORY_SIZE (4096)
#define MAX_LINE_SIZE (81) /*80 chars plus null terminator*/
#define INSTRUCTION_LENGTH (3)
#define MAX_INTEGER_LENGTH (5)
#define MAX_INTEGER_VALUE (2047)  /*the biggest number for a 12-bit size*/
#define MIN_INTEGER_VALUE (-2048) /*the smallest number for a 12-bit size*/
#define NUM_INSTRUCTIONS (16)
#define NUM_OF_DIRECTIVES (4)
#define REGISTER_LENGTH (2)
#define MAX_POSSIBLE_ADDRESSING_TYPE_FOR_TWO_OPERANDS (8)
#define NULL_TERMINATOR ('\0')
#define NEWLINE_CHARACTER ('\n') 

#define PRE_PROCESSOR_FILE_EXTENSION (".am")
#define SRC_FILE_EXTENSION (".as")
#define OBJ_FILE_EXTENSION (".ob")
#define ENT_FILE_EXTENSION (".ent")
#define EXT_FILE_EXTENSION (".ext")


#define ERR (-1)
#define USER_ERR (-2) /* USER_ERR means */
#define FUNC_ERR (-3) /* FUNC_ERR means in most of the times, failed to allocate memory using malloc func */
#define SUCCESS (0)

extern const char *REGISTERS_NAMES[NUM_OF_REGISTERS];
extern const char *INSTRUCTIONS_NAMES[NUM_INSTRUCTIONS];
extern const char *DIRECTIVES_NAMES[NUM_OF_DIRECTIVES];

#define PUSH_ERROR_AND_RETURN(log_error, ret_val, string_vector) \
	if (string_vector_push_back(string_vector, log_error) == ERR) \
	{ \
		LOG(ERROR, PARAMS, "string_vector_push_back failed"); \
		return ERR; \
	} \
	LOG(ERROR, PARAMS, log_error); \
	return ret_val; \

#define PUSH_ERROR_AND_LOG(log_error, string_vector) \
    { \
        if (string_vector_push_back(string_vector, log_error) == ERR) { \
            LOG(ERROR, PARAMS, "string_vector_push_back failed"); \
            return ERR; \
        } \
        LOG(ERROR, PARAMS, log_error); \
    }


#define PUSH_WARNING(log_warning, string_vector) \
	if (string_vector_push_back(string_vector, log_warning) == ERR) \
	{ \
		LOG(ERROR, PARAMS, "string_vector_push_back failed"); \
		return ERR; \
	} \
	LOG(WARNING, PARAMS, log_warning); \

#define CHECK_RET_VAL_AND_RETURN(ret_val) \
    do { \
        if (ret_val != SUCCESS) { \
            return ret_val; \
        } \
    } while (0)

#define CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(ret_val, log_msg, to_ret) \
	do { \
		if (ret_val == NULL) { \
			LOG(ERROR, PARAMS, log_msg); \
			return to_ret; \
		} \
	} while (0)

#endif /* GENERAL_CONST_H */
