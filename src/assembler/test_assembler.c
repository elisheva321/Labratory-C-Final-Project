/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file  test_assembler.c
 * @brief  This file contains the implementation of the 
 * tests for the assembler module.
 *********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "assembler.h"
#include "../assembler_main/assembler_main.h"
#include "../hash_table/hash_table.h"

void test_wrong_input_1()
{
    FILE *input_file;
    assembler_data_t *assembler_data = init_assembler_data();
    HashTable *macro_table = hash_table_init();
    if (assembler_data == NULL)
    {
        printf("FAILURE, assembler_data is NULL\n");
        return;
    }
    input_file = fopen("inputs_and_outputs_examples/invalid_inputs/wrong_input_instruction.am", "r");
    if (input_file == NULL)
    {
        printf("input file opening  failed\n");
        return;
    }
	printf("file opened successfully\n");
    assembler_first_pass(assembler_data, input_file, macro_table);
    assembler_second_pass(assembler_data, input_file);
	
    free_assembler_data(assembler_data);
}

void test_errors_vector()
{
    int i = 0;
    assembler_data_t *assembler_data = init_assembler_data();
    if (assembler_data == NULL)
    {
        printf("FAILURE, assembler_data is NULL\n");
        return;
    }

    for (i = 0; i < 11; i++)
    {
        string_vector_push_back(assembler_data->error_founds, "some error found");
    }

    for (i = 0; i < 12; i++)
    {
        string_vector_push_back(assembler_data->warning_founds, "some error found");
    }

    /* print both vectors content */
    for(i = 0; i < string_vector_get_size(assembler_data->error_founds); i++)
    {
        printf("error_founds[%d] = %s\n", i, string_vector_at(assembler_data->error_founds, i));
    }
    for(i = 0; i < string_vector_get_size(assembler_data->warning_founds); i++)
    {
        printf("warning_founds[%d] = %s\n", i, string_vector_at(assembler_data->warning_founds, i));
    }

    free_assembler_data(assembler_data);
}


int _test_code_instruction_mem_val(assembler_data_t *assembler_data, short address, short val)
{
    if (assembler_data->machine_binary_code[address] != val)
    {
        printf("\nFAILURE! expected val for code address %d is %d. actual : %d\n", address, val, assembler_data->machine_binary_code[address]);
        return ERR;
    }
    return SUCCESS;
}

int _test_code_data_mem_val(assembler_data_t *assembler_data, short address, short val)
{
    if (assembler_data->data_memory[address] != val)
    {
        printf("\nFAILURE! expected val for data address %d is %d. actual : %d\n", address, val, assembler_data->machine_binary_code[address]);
        return ERR;
    }
    return SUCCESS;
}
int convert14BitBinaryToDecimal(const char *binaryString)
{
    int result = 0;
    int negative = binaryString[0] == '1';
    int i;

    for (i = 1; i < 14; ++i)
    {
        result = result * 2 + (binaryString[i] == '1');
    }

    if (negative)
    {
        result -= (1 << 13);
    }

    return result;
}

void simple_test_first_pass()
{
    FILE *input_file;
    assembler_data_t *assembler_data = init_assembler_data();
    HashTable *macro_table = hash_table_init();
    int res = 0;
    if (assembler_data == NULL)
    {
        printf("FAILURE, assembler_data is NULL\n");
        return;
    }
    /* init macro_table */
	input_file = fopen("inputs_and_outputs_examples/am_files/input1.am", "r");
    if (input_file == NULL)
    {
        printf("inputfile openning failed");
        return;
    }
    assembler_first_pass(assembler_data, input_file, macro_table);

    if (res == SUCCESS)
    {
        printf("passed test successfully for input1_checked.am\n");
    }
    free_assembler_data(assembler_data);
    /*printf("SUCCESS, simple_test_assembler\n");*/
}

void simple_test_first_pass_check_binary()
{
    FILE *input_file;
    assembler_data_t *assembler_data = init_assembler_data();
    HashTable *macro_table = hash_table_init();
    int res = 0;
    if (assembler_data == NULL)
    {
        printf("FAILURE, assembler_data is NULL\n");
        return;
    }
    /* init macro_table */
    input_file = fopen("inputs_and_outputs_examples/am_files/input1_checked.am", "r");
    if (input_file == NULL)
    {
        printf("input file opening  failed");
        return;
    }
    assembler_first_pass(assembler_data, input_file, macro_table);

    /* MAIN:            mov     r3, LIST[sz] */
    res += _test_code_instruction_mem_val(assembler_data, 100, convert14BitBinaryToDecimal("00000000111000"));
    res += _test_code_instruction_mem_val(assembler_data, 101, convert14BitBinaryToDecimal("00000001100000"));
    res += _test_code_instruction_mem_val(assembler_data, 103, convert14BitBinaryToDecimal("00000000001000"));

    /* LOOP:  jmp L1 */
    res += _test_code_instruction_mem_val(assembler_data, 104, convert14BitBinaryToDecimal("00001001000100"));

    /*prn #-5  */
    res += _test_code_instruction_mem_val(assembler_data, 106, convert14BitBinaryToDecimal("00001100000000"));
    res += _test_code_instruction_mem_val(assembler_data, 107, convert14BitBinaryToDecimal("11111111101100"));

    /*  mov STR[5], STR[2]  */
    res += _test_code_instruction_mem_val(assembler_data, 108, convert14BitBinaryToDecimal("00000000101000"));
    res += _test_code_instruction_mem_val(assembler_data, 110, convert14BitBinaryToDecimal("00000000010100"));
    res += _test_code_instruction_mem_val(assembler_data, 112, convert14BitBinaryToDecimal("00000000001000"));

    /*  sub r1, r4  */
    res += _test_code_instruction_mem_val(assembler_data, 113, convert14BitBinaryToDecimal("00000011111100"));
    res += _test_code_instruction_mem_val(assembler_data, 114, convert14BitBinaryToDecimal("00000000110000"));

    /*  cmp     r3, #sz  */
    res += _test_code_instruction_mem_val(assembler_data, 115, convert14BitBinaryToDecimal("00000001110000"));
    res += _test_code_instruction_mem_val(assembler_data, 116, convert14BitBinaryToDecimal("00000001100000"));
    res += _test_code_instruction_mem_val(assembler_data, 117, convert14BitBinaryToDecimal("00000000001000"));

    /* bne  END */
    res += _test_code_instruction_mem_val(assembler_data, 118, convert14BitBinaryToDecimal("00001010000100"));

    /*  L1:  inc K  */
    res += _test_code_instruction_mem_val(assembler_data, 120, convert14BitBinaryToDecimal("00000111000100"));

    /* bne LOOP */
    res += _test_code_instruction_mem_val(assembler_data, 122, convert14BitBinaryToDecimal("00001010000100"));

    /* END:  hlt */
    res += _test_code_instruction_mem_val(assembler_data, 124, convert14BitBinaryToDecimal("00001111000000"));

    /* STR:  .string "abcdef" */
    res += _test_code_data_mem_val(assembler_data, 0, convert14BitBinaryToDecimal("00000001100001"));
    res += _test_code_data_mem_val(assembler_data, 1, convert14BitBinaryToDecimal("00000001100010"));
    res += _test_code_data_mem_val(assembler_data, 2, convert14BitBinaryToDecimal("00000001100011"));
    res += _test_code_data_mem_val(assembler_data, 3, convert14BitBinaryToDecimal("00000001100100"));
    res += _test_code_data_mem_val(assembler_data, 4, convert14BitBinaryToDecimal("00000001100101"));
    res += _test_code_data_mem_val(assembler_data, 5, convert14BitBinaryToDecimal("00000001100110"));
    res += _test_code_data_mem_val(assembler_data, 6, convert14BitBinaryToDecimal("00000000000000"));

    /* LIST: .data 6, -9, len */
    res += _test_code_data_mem_val(assembler_data, 7, convert14BitBinaryToDecimal("00000000000110"));
    res += _test_code_data_mem_val(assembler_data, 8, convert14BitBinaryToDecimal("11111111110111"));
    res += _test_code_data_mem_val(assembler_data, 9, convert14BitBinaryToDecimal("00000000000100"));

    if (res == SUCCESS)
    {
        printf("passed test successfully for input1_checked.am\n");
    }
    else
    {
        printf("FAILURE, simple_test_first_pass_check_binary\n");
    }
    free_assembler_data(assembler_data);
}

void test_first_second_pass_check_binary()
{
    FILE *input_file;
    assembler_data_t *assembler_data = init_assembler_data();
    HashTable *macro_table = hash_table_init();
    int res = 0;
    if (assembler_data == NULL)
    {
        printf("FAILURE, assembler_data is NULL\n");
        return;
    }
    /* init macro_table */
	/*input_file = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/am_files/input1_checked.am", "r");*/
    input_file = fopen("inputs_and_outputs_examples/am_files/input1_checked.am", "r");
    if (input_file == NULL)
    {
        printf("input file opening  failed\n");
        return;
    }
	printf("file opened successfully\n");
    assembler_first_pass(assembler_data, input_file, macro_table);

    assembler_second_pass(assembler_data, input_file);

    /* MAIN:            mov     r3, LIST[sz] */
    res += _test_code_instruction_mem_val(assembler_data, 100, convert14BitBinaryToDecimal("00000000111000"));
    res += _test_code_instruction_mem_val(assembler_data, 101, convert14BitBinaryToDecimal("00000001100000"));
    res += _test_code_instruction_mem_val(assembler_data, 102, convert14BitBinaryToDecimal("00001000010010"));
    res += _test_code_instruction_mem_val(assembler_data, 103, convert14BitBinaryToDecimal("00000000001000"));

    /* LOOP:  jmp L1 */
    res += _test_code_instruction_mem_val(assembler_data, 104, convert14BitBinaryToDecimal("00001001000100"));
    res += _test_code_instruction_mem_val(assembler_data, 105, convert14BitBinaryToDecimal("00000111100010"));


    /*prn #-5  */
    res += _test_code_instruction_mem_val(assembler_data, 106, convert14BitBinaryToDecimal("00001100000000"));
    res += _test_code_instruction_mem_val(assembler_data, 107, convert14BitBinaryToDecimal("11111111101100"));

    /*  mov STR[5], STR[2]  */
    res += _test_code_instruction_mem_val(assembler_data, 108, convert14BitBinaryToDecimal("00000000101000"));
    res += _test_code_instruction_mem_val(assembler_data, 109, convert14BitBinaryToDecimal("00000111110110"));
    res += _test_code_instruction_mem_val(assembler_data, 110, convert14BitBinaryToDecimal("00000000010100"));
    res += _test_code_instruction_mem_val(assembler_data, 111, convert14BitBinaryToDecimal("00000111110110"));
    res += _test_code_instruction_mem_val(assembler_data, 112, convert14BitBinaryToDecimal("00000000001000"));

    /*  sub r1, r4  */
    res += _test_code_instruction_mem_val(assembler_data, 113, convert14BitBinaryToDecimal("00000011111100"));
    res += _test_code_instruction_mem_val(assembler_data, 114, convert14BitBinaryToDecimal("00000000110000"));

    /*  cmp     r3, #sz  */
    res += _test_code_instruction_mem_val(assembler_data, 115, convert14BitBinaryToDecimal("00000001110000"));
    res += _test_code_instruction_mem_val(assembler_data, 116, convert14BitBinaryToDecimal("00000001100000"));
    res += _test_code_instruction_mem_val(assembler_data, 117, convert14BitBinaryToDecimal("00000000001000"));

    /* bne  END */
    res += _test_code_instruction_mem_val(assembler_data, 118, convert14BitBinaryToDecimal("00001010000100"));
    res += _test_code_instruction_mem_val(assembler_data, 119, convert14BitBinaryToDecimal("00000111110010"));

    /*  L1:  inc K  */
    res += _test_code_instruction_mem_val(assembler_data, 120, convert14BitBinaryToDecimal("00000111000100"));
    res += _test_code_instruction_mem_val(assembler_data, 121, convert14BitBinaryToDecimal("00001000011110"));


    /* bne LOOP */
    res += _test_code_instruction_mem_val(assembler_data, 122, convert14BitBinaryToDecimal("00001010000100"));
    res += _test_code_instruction_mem_val(assembler_data, 123, convert14BitBinaryToDecimal("00000110100010"));


    /* END:  hlt */
    res += _test_code_instruction_mem_val(assembler_data, 124, convert14BitBinaryToDecimal("00001111000000"));

    /* STR:  .string "abcdef" */
    res += _test_code_data_mem_val(assembler_data, 0, convert14BitBinaryToDecimal("00000001100001"));
    res += _test_code_data_mem_val(assembler_data, 1, convert14BitBinaryToDecimal("00000001100010"));
    res += _test_code_data_mem_val(assembler_data, 2, convert14BitBinaryToDecimal("00000001100011"));
    res += _test_code_data_mem_val(assembler_data, 3, convert14BitBinaryToDecimal("00000001100100"));
    res += _test_code_data_mem_val(assembler_data, 4, convert14BitBinaryToDecimal("00000001100101"));
    res += _test_code_data_mem_val(assembler_data, 5, convert14BitBinaryToDecimal("00000001100110"));
    res += _test_code_data_mem_val(assembler_data, 6, convert14BitBinaryToDecimal("00000000000000"));

    /* LIST: .data 6, -9, len */
    res += _test_code_data_mem_val(assembler_data, 7, convert14BitBinaryToDecimal("00000000000110"));
    res += _test_code_data_mem_val(assembler_data, 8, convert14BitBinaryToDecimal("11111111110111"));
    res += _test_code_data_mem_val(assembler_data, 9, convert14BitBinaryToDecimal("00000000000100"));

    if (res == SUCCESS)
    {
        printf("passed test successfully for input1_checked.am\n");
    }
    free_assembler_data(assembler_data);

}

void test_first_second_pass_check_binary_2()
{
    FILE *input_file;
    assembler_data_t *assembler_data = init_assembler_data();
    HashTable *macro_table = hash_table_init();
    int res = 0;
    if (assembler_data == NULL)
    {
        printf("FAILURE, assembler_data is NULL\n");
        return;
    }
    /* init macro_table */
	/*input_file = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/input1_checked.am", "r");*/
    input_file = fopen("inputs_and_outputs_examples/am_files/input1_checked_2.am", "r");
    if (input_file == NULL)
    {
        printf("input file openning  failed\n");
        return;
    }
	printf("file opened successfully\n");
    assembler_first_pass(assembler_data, input_file, macro_table);

    assembler_second_pass(assembler_data, input_file);

    /* MAIN:            mov     r3, LIST[sz] */
    res += _test_code_instruction_mem_val(assembler_data, 100, convert14BitBinaryToDecimal("00000000111000"));
    res += _test_code_instruction_mem_val(assembler_data, 101, convert14BitBinaryToDecimal("00000001100000"));
    res += _test_code_instruction_mem_val(assembler_data, 102, convert14BitBinaryToDecimal("00001000010010"));
    res += _test_code_instruction_mem_val(assembler_data, 103, convert14BitBinaryToDecimal("00000000001000"));

    /* LOOP:  jmp W */
    res += _test_code_instruction_mem_val(assembler_data, 104, convert14BitBinaryToDecimal("00001001000100"));
    res += _test_code_instruction_mem_val(assembler_data, 105, convert14BitBinaryToDecimal("00000000000001"));


    /*prn #-5  */
    res += _test_code_instruction_mem_val(assembler_data, 106, convert14BitBinaryToDecimal("00001100000000"));
    res += _test_code_instruction_mem_val(assembler_data, 107, convert14BitBinaryToDecimal("11111111101100"));

    /*  mov STR[5], STR[2]  */
    res += _test_code_instruction_mem_val(assembler_data, 108, convert14BitBinaryToDecimal("00000000101000"));
    res += _test_code_instruction_mem_val(assembler_data, 109, convert14BitBinaryToDecimal("00000111110110"));
    res += _test_code_instruction_mem_val(assembler_data, 110, convert14BitBinaryToDecimal("00000000010100"));
    res += _test_code_instruction_mem_val(assembler_data, 111, convert14BitBinaryToDecimal("00000111110110"));
    res += _test_code_instruction_mem_val(assembler_data, 112, convert14BitBinaryToDecimal("00000000001000"));

    /*  sub r1, r4  */
    res += _test_code_instruction_mem_val(assembler_data, 113, convert14BitBinaryToDecimal("00000011111100"));
    res += _test_code_instruction_mem_val(assembler_data, 114, convert14BitBinaryToDecimal("00000000110000"));

    /*  cmp     K, #sz  */
    res += _test_code_instruction_mem_val(assembler_data, 115, convert14BitBinaryToDecimal("00000001010000"));
    res += _test_code_instruction_mem_val(assembler_data, 116, convert14BitBinaryToDecimal("00001000011110"));
    res += _test_code_instruction_mem_val(assembler_data, 117, convert14BitBinaryToDecimal("00000000001000"));

    /* bne  W */
    res += _test_code_instruction_mem_val(assembler_data, 118, convert14BitBinaryToDecimal("00001010000100"));
    res += _test_code_instruction_mem_val(assembler_data, 119, convert14BitBinaryToDecimal("00000000000001"));

    /*  L1:  inc L3  */
    res += _test_code_instruction_mem_val(assembler_data, 120, convert14BitBinaryToDecimal("00000111000100"));
    res += _test_code_instruction_mem_val(assembler_data, 121, convert14BitBinaryToDecimal("00000000000001"));


    /* bne LOOP */
    res += _test_code_instruction_mem_val(assembler_data, 122, convert14BitBinaryToDecimal("00001010000100"));
    res += _test_code_instruction_mem_val(assembler_data, 123, convert14BitBinaryToDecimal("00000110100010"));


    /* END:  hlt */
    res += _test_code_instruction_mem_val(assembler_data, 124, convert14BitBinaryToDecimal("00001111000000"));

    /* STR:  .string "abcdef" */
    res += _test_code_data_mem_val(assembler_data, 0, convert14BitBinaryToDecimal("00000001100001"));
    res += _test_code_data_mem_val(assembler_data, 1, convert14BitBinaryToDecimal("00000001100010"));
    res += _test_code_data_mem_val(assembler_data, 2, convert14BitBinaryToDecimal("00000001100011"));
    res += _test_code_data_mem_val(assembler_data, 3, convert14BitBinaryToDecimal("00000001100100"));
    res += _test_code_data_mem_val(assembler_data, 4, convert14BitBinaryToDecimal("00000001100101"));
    res += _test_code_data_mem_val(assembler_data, 5, convert14BitBinaryToDecimal("00000001100110"));
    res += _test_code_data_mem_val(assembler_data, 6, convert14BitBinaryToDecimal("00000000000000"));

    /* LIST: .data 6, -9, len */
    res += _test_code_data_mem_val(assembler_data, 7, convert14BitBinaryToDecimal("00000000000110"));
    res += _test_code_data_mem_val(assembler_data, 8, convert14BitBinaryToDecimal("11111111110111"));
    res += _test_code_data_mem_val(assembler_data, 9, convert14BitBinaryToDecimal("00000000000100"));
    
    /* K: .data 22*/
    res += _test_code_data_mem_val(assembler_data, 10, convert14BitBinaryToDecimal("00000000010110"));


    if (res == SUCCESS)
    {
        printf("passed test successfully for input1_checked.am\n");
    }
    free_assembler_data(assembler_data);

}

/*
int main()

{
    printf("start testing assembler\n");

	test_wrong_input_1();

    simple_test_first_pass();
    simple_test_first_pass_check_binary();
    test_first_second_pass_check_binary();
    test_first_second_pass_check_binary_2();

    return 0;

}
*/

