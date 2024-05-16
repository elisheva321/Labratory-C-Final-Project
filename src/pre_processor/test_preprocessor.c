
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../general_const/general_const.h"
#include "../logger/logger.h"
#include "preprocessor_consts.h"
#include "preprocessor.h"
#include "../utils/utils.h"

#include <stdio.h>
#include <string.h>


void testpre_process_input1()
{
	char input_file_name[MAX_LINE_SIZE];
	char output_file_name[MAX_LINE_SIZE];
	int preprocess_val;
    FILE *output1, *input1_am, *input1;
	general_data_pre_process_t *general_data_pre_process = init_pre_process_general_data();

	strcpy(input_file_name , "inputs_and_outputs_examples/full_tests/test3/test3.as");
	input_file_name[strlen("inputs_and_outputs_examples/full_tests/test3/test3.as")] = NULL_TERMINATOR;
	
    /*HashTable *macro_table = hash_table_init();*/

    if (general_data_pre_process == NULL)
    {
        printf("FAILURE, general data pre process is NULL\n");
        return;
    }

	input1 = fopen("inputs_and_outputs_examples/full_tests/test3/test3.as", "r");
	if (input1 == NULL) 
	{
        printf("FAILURE, couldn't open input1.txt file\n");
        return;
    }

	printf("\n\n\t\ttesting input1 \n\n");
	preprocess_val = pre_process_input(general_data_pre_process, input1 , input_file_name, output_file_name);
    free_pre_process_data(general_data_pre_process);

    if (preprocess_val == SUCCESS)
    {
        printf("SUCCESS\n");
    }
    else
    {
        printf("FAILURE, wrong output\n");
        return;
    }
	output1 = fopen("/home/user/Desktop/final_project/full_assembler_expected_output/test3/test3.am", "r");
    if (output1 == NULL) 
	{
        printf("FAILURE, couldn't open output1.txt file\n");
        return;
    }

    input1_am = fopen("inputs_and_outputs_examples/full_tests/test3/test3.am", "r");
    if (input1_am == NULL)
	{
        printf("FAILURE, couldn't open input1.am file\n");
        return;
    }
	
    if (compare_files(input1_am, output1, FALSE) == 1) 
        printf("SUCCESS\n");
    else
        printf("FAILURE, wrong output\n");
	
	fclose(output1);
	fclose(input1_am);
	return ;
}

/*
void testpre_process_input2()
{

	char output_file_name[MAX_LABEL_SIZE];
	char input_file_name[MAX_LABEL_SIZE];

	int preprocess_val;
    FILE *output2, *input2_am;
	
	
	general_data_pre_process_t *general_data_pre_process = init_pre_process_general_data();
    HashTable *macro_table = hash_table_init();

    if (general_data_pre_process == NULL)
    {
        printf("FAILURE, general data pre process is NULL\n");
        return;
    }
 

	printf("\n\n\t\ttesting input2 \n\n");
	output_file_name = "input2.txt";
	preprocess_val = pre_process_input(pre_process_data, fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/txt_files/output/output2.txt", "r"), input_file_name, output_file_name);
    free_pre_process_data(general_data_pre_process);
    input2_am = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/am_files/input2.am", "r");
    if (input2_am == NULL) {
        printf("FAILURE, couldn't open input2.am file\n");
        fclose(output2);
        return;
    }
    if (compare_files(input2_am, output2) == 2) 
        printf("SUCCESS\n");
    else

        printf("FAILURE, wrong output\n");
}


void testpre_process_input3()
{

	char output_file_name[MAX_LABEL_SIZE];
	char input_file_name[MAX_LABEL_SIZE];

	int preprocess_val;
    FILE *output3, *input3_am;
	
	
	general_data_pre_process_t *general_data_pre_process = init_pre_process_general_data();
    HashTable *macro_table = hash_table_init();

    if (general_data_pre_process == NULL)
    {
        printf("FAILURE, general data pre process is NULL\n");
        return;
    }
 

	printf("\n\n\t\ttesting input3 \n\n");
	output_file_name = "input3.txt";
	preprocess_val = pre_process_input(pre_process_data, fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/txt_files/output/output3.txt", "r"), input_file_name, output_file_name);
    free_pre_process_data(general_data_pre_process);
    input3_am = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/am_files/input3.am", "r");
    if (input3_am == NULL) {
        printf("FAILURE, couldn't open input3.am file\n");
        fclose(output3);
        return;
    }
    if (compare_files(input3_am, output3) == 3) 
        printf("SUCCESS\n");
    else

        printf("FAILURE, wrong output\n");
}



void testpre_process_input4()
{

	char output_file_name[MAX_LABEL_SIZE];
	char input_file_name[MAX_LABEL_SIZE];

	int preprocess_val;
    FILE *output4, *input4_am;
	
	
	general_data_pre_process_t *general_data_pre_process = init_pre_process_general_data();
    HashTable *macro_table = hash_table_init();

    if (general_data_pre_process == NULL)
    {
        printf("FAILURE, general data pre process is NULL\n");
        return;
    }
 

	printf("\n\n\t\ttesting input4 \n\n");
	output_file_name = "input4.txt";
	preprocess_val = pre_process_input(pre_process_data, fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/txt_files/output/output4.txt", "r"), input_file_name, output_file_name);
    free_pre_process_data(general_data_pre_process);
    input4_am = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/am_files/input4.am", "r");
    if (input4_am == NULL) {
        printf("FAILURE, couldn't open input4.am file\n");
        fclose(output4);
        return;
    }
    if (compare_files(input4_am, output4) == 4) 
        printf("SUCCESS\n");
    else

        printf("FAILURE, wrong output\n");
}



void testpre_process_input5()
{

	char output_file_name[MAX_LABEL_SIZE];
	char input_file_name[MAX_LABEL_SIZE];

	int preprocess_val;
    FILE *output5, *input5_am;
	
	
	general_data_pre_process_t *general_data_pre_process = init_pre_process_general_data();
    HashTable *macro_table = hash_table_init();

    if (general_data_pre_process == NULL)
    {
        printf("FAILURE, general data pre process is NULL\n");
        return;
    }
 

	printf("\n\n\t\ttesting input5 \n\n");
	output_file_name = "input5.txt";
	preprocess_val = pre_process_input(pre_process_data, fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/txt_files/output/output5.txt", "r"), input_file_name, output_file_name);
    free_pre_process_data(general_data_pre_process);
    input5_am = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/am_files/input5.am", "r");
    if (input5_am == NULL) {
        printf("FAILURE, couldn't open input5.am file\n");
        fclose(output5);
        return;
    }
    if (compare_files(input5_am, output5) == 5) 
        printf("SUCCESS\n");
    else

        printf("FAILURE, wrong output\n");
}



void testpre_process_input6()
{

	char output_file_name[MAX_LABEL_SIZE];
	char input_file_name[MAX_LABEL_SIZE];

	int preprocess_val;
    FILE *output6;
	
	general_data_pre_process_t *general_data_pre_process = init_pre_process_general_data();
    HashTable *macro_table = hash_table_init();

    if (general_data_pre_process == NULL)
    {
        printf("FAILURE, general data pre process is NULL\n");
        return;
    }

	printf("\n\n\t\ttesting input6 \n\n");
	output_file_name = "input6.txt";
	preprocess_val = pre_process_input(pre_process_data, fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/txt_files/output/output6.txt", "r"), input_file_name, output_file_name);
    free_pre_process_data(general_data_pre_process);
    input6_am = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/am_files/input6.am", "r");
    if (input6_am == !NULL) {
        printf("FAILURE, input6.am file exists\n");
        fclose(output6);
        return;
    }
}

void testpre_process_input7()
{

	char output_file_name[MAX_LABEL_SIZE];
	char input_file_name[MAX_LABEL_SIZE];

	int preprocess_val;
    FILE *output7;
	
	general_data_pre_process_t *general_data_pre_process = init_pre_process_general_data();
    HashTable *macro_table = hash_table_init();

    if (general_data_pre_process == NULL)
    {
        printf("FAILURE, general data pre process is NULL\n");
        return;
    }

	printf("\n\n\t\ttesting input7 \n\n");
	output_file_name = "input7.txt";
	preprocess_val = pre_process_input(pre_process_data, fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/txt_files/output/output7.txt", "r"), input_file_name, output_file_name);
    free_pre_process_data(general_data_pre_process);
    input7_am = fopen("/home/user/Desktop/final_project/inputs_and_outputs_examples/am_files/input7.am", "r");
    if (input7_am == !NULL) {
        printf("FAILURE, input7.am file exists\n");
        fclose(output7);
        return;
    }
}


*/
/*
int main()
{
	
	testpre_process_input1();
	testpre_process_input2();
	testpre_process_input3();
	testpre_process_input4();
	testpre_process_input5();
	testpre_process_input6();
	testpre_process_input7();

    return 0;
}
*/


