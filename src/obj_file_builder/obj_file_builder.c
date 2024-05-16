/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  obj_file_builder.c
 * @brief  This file contains the implementation of the obj file builder.
 * 
 *********************************************************************/

#include "obj_file_builder.h"

#include "../utils/utils.h"
#include "../general_const/general_const.h"

#define OBJ_FILE_BASE (4)
#define WORD_CONTENT_SIZE (7)

const char encrypted_curr_lut[OBJ_FILE_BASE] = {'*', '#', '%', '!'};

/* convert a number to base 4 */
void _convert_to_base4_encrypted(short num, char *encrypted_word, short size)
{
    short i = 0;
    unsigned short curr_num = num;
    short temp = 0;

    for (i = size - 1; i >= 0; --i)
    {
        temp = curr_num % OBJ_FILE_BASE;
        encrypted_word[i] = encrypted_curr_lut[temp];
        curr_num /= OBJ_FILE_BASE;
    }
}

int build_obj_file(const char *filename, assembler_data_t *assembler_data)
{
    /* first line include IC and DC */
    short curr_address = 0;
    short curr_content = 0;
    char encrypted_word[WORD_CONTENT_SIZE];
    char output_file_name[MAX_LINE_SIZE];
    FILE *output_file = NULL;
    int i, j , k= 0;
	
    get_output_file_name(filename, output_file_name, OBJ_FILE_EXTENSION);

    output_file = fopen(output_file_name, "w");
    if (output_file == NULL)
    {
        return FUNC_ERR;
    }
    fprintf(output_file, "  %d %d\n", assembler_data->IC - IC_INITIAL_VALUE, assembler_data->DC);

    /* each line include the address of a word and the content of the word.
     address will be a number base decimal.
    content will be a number base 4 encrypted. */

    /* building instruction memory part */
    for (i = IC_INITIAL_VALUE; i < assembler_data->IC; i++)
    {
        curr_address = i;
        curr_content = assembler_data->machine_binary_code[i];
        _convert_to_base4_encrypted(curr_content, encrypted_word, WORD_CONTENT_SIZE);
        if (count_digit(curr_address) < 4)
        {
            fprintf(output_file, "0");
        }
        fprintf(output_file, "%d ", curr_address);
        for(j = 0; j < WORD_CONTENT_SIZE; j++)
        {
            fprintf(output_file, "%c", encrypted_word[j]);
        }
        fprintf(output_file, "\n");
    }

    /* building data memory part */
    for (j = 0; j < assembler_data->DC; j++)
    {
        curr_address = i;
        curr_content = assembler_data->data_memory[j];
        _convert_to_base4_encrypted(curr_content, encrypted_word, WORD_CONTENT_SIZE);
        if (count_digit(curr_address) < 4)
        {
            fprintf(output_file, "0");
        }
		fprintf(output_file, "%d ", curr_address);
		for(k = 0; k < WORD_CONTENT_SIZE; k++)
        {
            fprintf(output_file, "%c", encrypted_word[k]);
        }
        if (j < assembler_data->DC - 1)
        {
            fprintf(output_file, "\n");
        }
		++i;
    }
    fclose(output_file);
    return SUCCESS;
}
