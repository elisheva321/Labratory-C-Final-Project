/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file assembler_main.c
 * @brief This file contains the implementation of the main assembler functions.
 * This is is the entry point of the assembler.
 *********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "assembler_main.h"
#include "../assembler/assembler.h"
#include "../pre_processor/preprocessor.h"
#include "../pre_processor/preprocessor_consts.h"
#include "../logger/logger.h"
#include "../general_const/general_const.h"
#include "../obj_file_builder/obj_file_builder.h"
#include "../ext_ent_file_builder/ext_ent_file_builder.h"
#define FILE_EXTENSION_SIZE (4)
/*========================internal function declaration ========================*/

/* print massages of type warning/error to the user */
int _print_massages(StringVector *msg_vec, enum msg_type msg_type);
int _print_errors_and_warnings(StringVector *errors_found, StringVector *warnings_found, const char*input_file);

/*========================data initialization and freeing========================*/

int _assembler_input(const char *input_file_name, general_data_pre_process_t *pre_process_data, assembler_data_t *assembler_data)
{
	int ret = SUCCESS;
	int file_name_size = strlen(input_file_name);
	char *pre_process_input_file_name = NULL;
	char *pre_process_output_file_name = NULL;
	
	FILE *pre_process_output_file = NULL;
	FILE *pre_process_input_file = NULL;
	int should_create_output_files = TRUE;
	
	pre_process_input_file_name = (char *)malloc(file_name_size + FILE_EXTENSION_SIZE + 1);
	CHECK_RET_VAL_PTR_AND_LOG_AND_RETURN(pre_process_input_file_name, "Memory allocation failed in _assembler_input for pre_process_input_file_name", FUNC_ERR);

	get_output_file_name(input_file_name, pre_process_input_file_name, SRC_FILE_EXTENSION);
	pre_process_input_file = fopen(pre_process_input_file_name, "r");

	if (pre_process_input_file == NULL)
	{
		free(pre_process_input_file_name);
		LOG(ERROR, PARAMS, "Error opening file %s", pre_process_input_file_name);
		printf("failed on opening file %s, will not continue to the assembler process \n", pre_process_input_file_name);
		return USER_ERR;
	}

	pre_process_output_file_name = (char *)malloc(file_name_size + FILE_EXTENSION_SIZE + 1);
	if (pre_process_output_file_name == NULL)
	{
		free(pre_process_input_file_name);
		fclose(pre_process_input_file);
		LOG(ERROR, PARAMS, "Memory allocation failed in _assembler_input for pre_process_output_file_name");
		return FUNC_ERR;
	}
	ret = pre_process_input(pre_process_data, pre_process_input_file, input_file_name, pre_process_output_file_name);
	if (ret != SUCCESS)
	{
		free(pre_process_input_file_name);
		free(pre_process_output_file_name);
		LOG(ERROR, PARAMS, "Error pre processing file %s", input_file_name);
		ret = _print_errors_and_warnings(pre_process_data->error_founds, pre_process_data->warning_founds, input_file_name);
		if (ret == FUNC_ERR)
		{
			return ret;
		}
		else if (ret == USER_ERR)
		{
			printf("failed on pre processing file %s, will not continue for first pass\n", input_file_name);
			return USER_ERR;
		}
			return ret;
	}

	free(pre_process_input_file_name);

	pre_process_output_file = fopen(pre_process_output_file_name, "r");
	if (pre_process_output_file == NULL)
	{
		LOG(ERROR, PARAMS, "Error opening file %s", pre_process_output_file_name);
		return FUNC_ERR;
	}

	LOG(INFO, PARAMS, "starting assembler first pass on %s.", pre_process_output_file_name);
	ret = assembler_first_pass(assembler_data, pre_process_output_file, pre_process_data->macro_table);
	if (ret == FUNC_ERR)
	{
		LOG(ERROR, PARAMS, "Error in first pass processing file %s", input_file_name);
		return ret;
	}
	else if (ret == USER_ERR)
	{
		should_create_output_files = FALSE;
	}

	ret = assembler_second_pass(assembler_data, pre_process_output_file);
	if (ret == FUNC_ERR)
	{
		LOG(ERROR, PARAMS, "Error in second pass processing file %s", input_file_name);
		return ret;
	}
	ret = _print_errors_and_warnings(assembler_data->error_founds, assembler_data->warning_founds, input_file_name);
	if (ret == FUNC_ERR)
	{
		return ret;
	}
	else if (ret == USER_ERR)
	{
		should_create_output_files = FALSE;
	}

	if (should_create_output_files)
	{
		build_obj_file(input_file_name, assembler_data);
		build_entry_file(input_file_name, assembler_data);
		build_extern_file(input_file_name, assembler_data);
	}

	free(pre_process_output_file_name);
	fclose(pre_process_output_file);
	
	if (ret != SUCCESS)
	{
		LOG(ERROR, PARAMS, "Error in assembler_input function file: %s", input_file_name);
	}
	return ret;
}

void _free_pre_process_data(general_data_pre_process_t *pre_process_data)
{
	if (pre_process_data != NULL)
	{
		free_pre_process_data(pre_process_data);
	}
}
void _free_assembler_data(assembler_data_t *assembler_data)
{
	if (assembler_data != NULL)
	{
		free_assembler_data(assembler_data);
	}
}
int _destruct_assembler_all_data(general_data_pre_process_t *pre_process_data, assembler_data_t *assembler_data)
{
	if (pre_process_data != NULL)
	{
		_free_pre_process_data(pre_process_data);
	}
	if (assembler_data != NULL)
	{
		_free_assembler_data(assembler_data);
	}
	return SUCCESS;
}

void _clear_assembler_all_data(general_data_pre_process_t *pre_process_data, assembler_data_t *assembler_data)
{
	if (pre_process_data != NULL)
	{
		clear_pre_process_data(pre_process_data);
	}
	if (assembler_data != NULL)
	{
		clear_assembler_data(assembler_data);
	}
}

int assembler_inputs(const char *input_file_names[], int num_input_files)
{
	int i = 0;
	int ret_val = SUCCESS;
	general_data_pre_process_t *pre_process_data = NULL;
	assembler_data_t *assembler_data = NULL;

	/* initialize pre processor general data*/
	pre_process_data = init_pre_process_general_data();
	if (pre_process_data == NULL)
	{
		LOG(ERROR, PARAMS, "Error initializing pre process data");
		return ERROR;
	}

	assembler_data = init_assembler_data();
	if (assembler_data == NULL)
	{
		LOG(ERROR, PARAMS, "Error initializing assembler_data data");
		return ERROR;
	}

	for (i = 0; i < num_input_files; ++i)
	{
		ret_val += _assembler_input(input_file_names[i], pre_process_data, assembler_data);
		_clear_assembler_all_data(pre_process_data, assembler_data);
	}
	_destruct_assembler_all_data(pre_process_data, assembler_data);
	
	return ret_val == SUCCESS ? SUCCESS : ERR;
}

int _print_massages(StringVector *msg_vec, enum msg_type msg_type)
{
	int i;
	int msg_num = string_vector_get_size(msg_vec);
	const char *msg = NULL;
	const char *msg_type_content = msg_type == MSG_TYPE__ERROR ? "Errors:" : "Warnings:";
	int ret_val = SUCCESS;
	if (msg_num != 0)
	{
		printf("%s\n", msg_type_content);
		for (i = 0; i < msg_num; i++)
		{
			msg = string_vector_at(msg_vec, i);
			if (msg == NULL)
			{
				LOG(ERROR, PARAMS, "string_vector_at failed at _print_massages");
				return FUNC_ERR;
			}
			ret_val = USER_ERR;
			printf("%s\n", msg);
		}
	}
	return ret_val;
}


int _print_errors_and_warnings(StringVector *errors_found, StringVector *warnings_found, const char *input_file)
{
	int ret = SUCCESS;

	if (string_vector_get_size(errors_found) > 0)
	{
		LOG(INFO, PARAMS, "failed on first pass. Errors:  %s.", input_file);
		ret = _print_massages(errors_found, MSG_TYPE__ERROR);
		if (ret == FUNC_ERR)
		{
			LOG(ERROR, PARAMS, "_print_massages function failed");
			return FUNC_ERR;
		}
	}
	if (_print_massages(warnings_found, MSG_TYPE__WARNING) != SUCCESS)
	{
		LOG(ERROR, PARAMS, "_print_massages function failed");
		return FUNC_ERR;
	}
	return ret;
}


/**
 * @brief The entry point of the program.
 *
 * This function is the starting point of the program execution. It takes command line arguments
 * and returns an integer value indicating the exit status of the program.
 *
 * @param argc The number of command line arguments.
 * @param argv An array of strings containing the command line arguments.
 * @return An integer value indicating the exit status of the program.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

	return assembler_inputs((const char **)argv + 1, argc - 1);
}

