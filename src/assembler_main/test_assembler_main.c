/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota
 *
 * @file  test_assembler_main.c
 * @brief This file contains the implementation of the tests for the assembler main functions.
 *
 *********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "../hash_table/hash_table.h"
#include "../assembler/assembler.h"
#include "assembler_main.h"


void test_invalid_input_2()
{
    const char *input_file_name = "invalid_input/error1/error1";

    if (SUCCESS == assembler_inputs(&input_file_name, 1))
    {
        printf("FAILURE, assembler_inputs\n");
        return;
    }

	if( fopen("invalid_input/error1/error1.ob", "r") != NULL)
	{
        printf("FAILURE object file was created\n");
		return ;
    }

	if( fopen("invalid_input/error1/error1.ent", "r") != NULL)
	{
        printf("FAILURE entry file was created\n");
		return ;
    }

	if( fopen("invalid_input/error1/error1.ext", "r") != NULL)
	{
        printf("FAILURE entry file was created\n");
		return ;
    }

	printf("SUCCESS test_invalid_input_2\n");
	return;
}


void test_valid_input_6() 
{
    const char *input_file_name = "valid_input/test6/test6";
    int res = 0;

    if (SUCCESS != assembler_inputs(&input_file_name, 1))
    {
        printf("FAILURE, assembler_inputs\n");
        return;
    }

    res += compare_files(fopen("valid_input/test6/test6.am", "r"), fopen("expected_output/test6/test6.am", "r"), TRUE);
    res += compare_files(fopen("valid_input/test6/test6.ob", "r"), fopen("expected_output/test6/test6.ob", "r"), TRUE);

    if (res == SUCCESS)
    {
        printf("SUCCESS, test_valid_input_6\n");
    }
    else
    {
        printf("FAILURE, test_valid_input_6\n");
    }
	return;
}


void test_valid_input_5() 
{
    const char *input_file_name = "valid_input/test5/test5";
    int res = 0;

    if (SUCCESS != assembler_inputs(&input_file_name, 1))
    {
        printf("FAILURE, assembler_inputs\n");
        return;
    }
    res += compare_files(fopen("valid_input/test5/test5.am", "r"), fopen("expected_output/test5/test5.am", "r"), TRUE);
    res += compare_files(fopen("valid_input/test5/test5.ob", "r"), fopen("expected_output/test5/test5.ob", "r"), TRUE);
    res += compare_files(fopen("valid_input/test5/test5.ent", "r"), fopen("expected_output/test5/test5.ent", "r"), TRUE);
    res += compare_files(fopen("valid_input/test5/test5.ext", "r"), fopen("expected_output/test5/test5.ext", "r"), TRUE);

    if (res == SUCCESS)
    {
        printf("SUCCESS, test_valid_input_5\n");
    }
    else
    {
        printf("FAILURE, test_valid_input_5\n");
    }
	return;
}


void test_invalid_input_3() /*success*/
{
	const char *input_file_name = "invalid_input/test4/test4";

    if (SUCCESS == assembler_inputs(&input_file_name, 1))
    {
        printf("FAILURE assembler_inputs\n");
    }
	
    if( fopen("invalid_input/test4/test4.am", "r") == NULL)
	{
        printf("FAILURE am file was not created\n");
		return ;
    }

	if( fopen("invalid_input/test4/test4.ob", "r") != NULL)
	{
        printf("FAILURE object file was created\n");
		return ;
    }

	if( fopen("invalid_input/test4/test4.ent", "r") != NULL)
	{
        printf("FAILURE entry file was created\n");
		return ;
    }

	if( fopen("invalid_input/test4/test4.ext", "r") != NULL)
	{
        printf("FAILURE extern file was created\n");
		return ;
    }

	printf("SUCCESS test_invalid_input_3\n");
	return;
}


void test_valid_input_2() /*success*/
{
    int res = 0;
    const char *input_file_name = "valid_input/test1/test1";

    if (SUCCESS != assembler_inputs(&input_file_name, 1))
    {
        printf("FAILURE, assembler_inputs\n");
        return;
    }

    res += compare_files(fopen("valid_input/test1/test1.am", "r"), fopen("expected_output/test1/test1.am", "r"), TRUE);
    res += compare_files(fopen("valid_input/test1/test1.ob", "r"), fopen("expected_output/test1/test1.ob", "r"), TRUE);
    res += compare_files(fopen("valid_input/test1/test1.ent", "r"), fopen("expected_output/test1/test1.ent", "r"), TRUE);
    res += compare_files(fopen("valid_input/test1/test1.ext", "r"), fopen("expected_output/test1/test1.ext", "r"), TRUE);

    if (res == SUCCESS)
    {
        printf("SUCCESS, test_valid_input_2\n");
    }
    else
    {
        printf("FAILURE, test_valid_input_2\n");
    }

	printf("SUCCESS\n");
	return;
}


void test_invalid_input_1() /*success*/
{
    const char *input_file_name = "invalid_input/test2/test2";
    int res = 0;

    res = assembler_inputs(&input_file_name, 1);

    if (res == SUCCESS)
    {
        printf("FAILURE test_valid_input_3\n");
    }

	if( fopen("invalid_input/test2/test2.ob", "r") != NULL)
	{
        printf("FAILURE object file was created\n");
		return ;
    }

	if( fopen("invalid_input/test2/test2.ent", "r") != NULL)
	{
        printf("FAILURE entry file was created\n");
		return ;
    }

	if( fopen("invalid_input/test2/test2.ext", "r") != NULL)
	{
        printf("FAILURE extern file was created\n");
		return;
    }

	printf("SUCCESS test_valid_input_1\n");
	return;
}

void test_valid_input_4()
{
    const char *input_file_name = "valid_input/test3/test3";
    int res = 0;

    if (SUCCESS != assembler_inputs(&input_file_name, 1))
    {
        return;
    }

    res += compare_files(fopen("valid_input/test3/test3.am", "r"), fopen("expected_output/test3/test3.am", "r"), TRUE);
    res += compare_files(fopen("valid_input/test3/test3.ob", "r"), fopen("expected_output/test3/test3.ob", "r"), TRUE);
    res += compare_files(fopen("valid_input/test3/test3.ent", "r"), fopen("expected_output/test3/test3.ent", "r"), TRUE);
    res += compare_files(fopen("valid_input/test3/test3.ext", "r"), fopen("expected_output/test3/test3.ext", "r"), TRUE);

    if (res != SUCCESS)
    {
        printf("FAILURE, test_valid_input_4\n");
    }
    else
    {
        printf("SUCCESS, test_valid_input_4\n");
    }

	return;
}

void test_valid_input_1()
{
    const char *input_file_name = "valid_input/ps1/ps1";
    int res = 0;
    FILE *file;

    if (SUCCESS != assembler_inputs(&input_file_name, 1))
    {
        printf("FAILURE, assembler_inputs test_valid_input_1\n");
        return;
    }

    file = fopen("valid_input/ps1/ps1.am", "r");
    if (file == NULL)
    {
        printf("couldn't open valid_input/ps1/ps1.am file");
        return;
    }

    res += compare_files(fopen("valid_input/ps1/ps1.ob", "r"), fopen("expected_output/ps1/ps1.ob", "r"), TRUE);
    res += compare_files(fopen("valid_input/ps1/ps1.ent", "r"), fopen("expected_output/ps1/ps1.ent", "r"), TRUE);
    res += compare_files(fopen("valid_input/ps1/ps1.ext", "r"), fopen("expected_output/ps1/ps1.ext", "r"), TRUE);

    if (res == SUCCESS)
    {
        printf("SUCCESS, test_valid_input_1\n");
    }
    else
    {
        printf("FAILURE, test_valid_input_1\n");
    }
    fclose(file);
    return;
}

void test_invalid_input_4()
{
    const char *input_file_name = "invalid_input/test7/test7";

    if (SUCCESS == assembler_inputs(&input_file_name, 1))
    {
        printf("FAILURE, assembler_inputs\n");
        return;
    }
    if( fopen("invalid_input/test7/test7.am", "r") != NULL)
    {
        printf("FAILURE am file was created\n");
        return ;
    }

    if( fopen("invalid_input/test7/test7.ob", "r") != NULL)
    {
        printf("FAILURE object file was created\n");
        return ;
    }

    if( fopen("invalid_input/test7/test7.ent", "r") != NULL)
    {
        printf("FAILURE entry file was created\n");
        return ;
    }

    if( fopen("invalid_input/test7/test7.ext", "r") != NULL)
    {
        printf("FAILURE extern file was created\n");
        return;
    }

    printf("SUCCESS test_invalid_input_7\n");
    return;
}


void test_valid_inputs()
{
    test_valid_input_1();
    test_valid_input_2();
    test_valid_input_4();
    test_valid_input_6();
    test_valid_input_5();
}

void test_invalid_inputs()
{
    test_invalid_input_1();
    test_invalid_input_2();
    test_invalid_input_3();
    test_invalid_input_4();
}


/* uncomment this line to run test_assembler_main */
/*int main()
{
    test_valid_inputs();
    test_invalid_inputs();
    return SUCCESS;
}*/

