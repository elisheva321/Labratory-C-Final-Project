/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file ext_ent_file_builder.c
 * @brief This file contains the implementation of the functions
 * that build the extern and entry files.
 * 
 *********************************************************************/

#include <stdio.h>

#include "ext_ent_file_builder.h"
#include "../hash_table/hash_table.h"
#include "../assembler/assembler.h"
#include "../utils/utils.h"
#include "../logger/logger.h"

/* -------------------------------- internal function declarations -------------*/
void _write_to_file_if_symbol_entry(const char *key, void *data, void *file);

/*--------------------------------- internal function definitions -------------*/
void _write_to_file_if_symbol_entry(const char *key, void *data, void *file)
{
    symbol_t *symbol = (symbol_t *)data;
    if (symbol->symbol_location == SYMBOL__ENTRY)
    {
        fprintf(file, "%s\t", symbol->symbol_name);
		if (count_digit(symbol->symbol_value) < 4)
        {
            fprintf(file, "0");
        }
		fprintf(file, "%d\n", symbol->symbol_value);
    }
}

int build_extern_file(const char *filename, assembler_data_t *assembler)
{
    char output_filename[MAX_LINE_SIZE];
    FILE *file;
    extern_user_t *extern_user;

    if (queue_is_empty(assembler->extern_symbols))
    {
        /* no externs were defined in the file, no need to create the file */
        return SUCCESS;
    }
    get_output_file_name(filename, output_filename, EXT_FILE_EXTENSION);
    file = fopen(output_filename, "w");

    while (!queue_is_empty(assembler->extern_symbols))
    {
        extern_user  = (extern_user_t *)queue_dequeue(assembler->extern_symbols);
		if (extern_user == NULL)
        {
            LOG(ERROR, PARAMS, "queue_dequeue failed");
            fclose(file);
            return FUNC_ERR;
        }
		fprintf(file, "%s\t", extern_user->label_name);
		if (count_digit(extern_user->address) < 4)
        {
            fprintf(file, "0");
        }
		fprintf(file, "%d\n", extern_user->address);

        free(extern_user);
    }

    fclose(file);
    return SUCCESS;
}

int build_entry_file(const char *filename, assembler_data_t *assembler_data)
{
    char output_filename[MAX_LINE_SIZE];
    FILE *output_file = NULL;

    if (assembler_data->has_entry == FALSE)
    {
        /* no entry was defined in the file, no need to create the file */
        return SUCCESS;
    }

    get_output_file_name(filename, output_filename, ENT_FILE_EXTENSION);
    output_file = fopen(output_filename, "w");
    hash_table_for_each(assembler_data->symbol_table, _write_to_file_if_symbol_entry, output_file);

    fclose(output_file);
    return 0;
}




