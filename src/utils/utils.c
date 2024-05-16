/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file utils.c
 * @brief This file contains utility functions declarations.
 * 
 *********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>


#include "utils.h"

#include "../string_vector/string_vector.h"
#include "../logger/logger.h"
#include "../general_const/general_const.h"

char *str_dup(const char *s)
{
	size_t len = strlen(s) + 1;
	char *dup = malloc(len);
	if (dup != NULL)
	{
		strcpy(dup, s);
	}
	return dup;
}

unsigned short twos_complement_short(short num)
{
	return ~num + 1;
}

short set_bits_short(short num, short value_to_set_at_specific_bits, short num_bits, short starting_bit)
{
	/* Create a mask with the lowest `num_bits` set to 1 */
	short mask = (1U << num_bits) - 1;

	/* Shift the mask to align with the starting_bit position */
	mask <<= starting_bit;

	/* Align the value to set with the starting_bit, ensuring we only keep the  lowest `num_bits` */
	value_to_set_at_specific_bits = (value_to_set_at_specific_bits & ((1U << num_bits) - 1)) << starting_bit;

	/* Clear the bits at the target positions in 'num' */
	num &= ~mask;

	/* Set the bits to the new value */
	num |= value_to_set_at_specific_bits;

	return num;
}

short string_to_short(const char *str, int *error)
{
	long num;
	char *end;

	num = strtol(str, &end, 10); /* Base 10 for decimal numbers */

	/* Check for conversion errors: no digits were found,
	   remaining characters after number, or value out of short range */
	if (end == str || *end != '\0' || num > SHRT_MAX || num < SHRT_MIN)
	{
		*error = ERR;
		return 0; /* Return value is irrelevant when error is indicated */
	}

	*error = SUCCESS;						  /* No errors */
	return twos_complement_short((short)num); /* Safely cast to short */
}

void get_output_file_name(const char *input_file_name, char *output_file_name, const char *postfix)
{
	const char *dot_position = strrchr(input_file_name, '.');
	if (dot_position != NULL)
	{
		size_t length = dot_position - input_file_name;
		strncpy(output_file_name, input_file_name, length);
		strcpy(output_file_name + length, postfix);
		LOG(INFO, PARAMS, "output file name: %s \n", output_file_name);
	}
	else
	{
		strcpy(output_file_name, input_file_name);
		strcat(output_file_name, postfix);
		LOG(INFO, PARAMS, "output file name: %s \n", output_file_name);
	}
}

short count_digit(short num)
{
    short count = 0;
    while (num != 0)
    {
        num /= 10;
        count++;
    }
    return count;
}

int compare_files(FILE *file1, FILE *file2, int should_close_files)
{
    char ch1, ch2;
	int ret_val = SUCCESS;

	if (file1 == NULL)
	{
		LOG(ERROR, PARAMS, "file1 is NULL");
		return ERROR;
	}
	if (file2 == NULL)
	{
		LOG(ERROR, PARAMS, "file2 is NULL");
		return ERROR;
	}
   
    do {
        ch1 = fgetc(file1);
        ch2 = fgetc(file2);

        if (ch1 != ch2) 
		{	
			printf("%c , %c\n", ch1, ch2);
			if (should_close_files)
			{
				fclose(file1);
				fclose(file2);
			}
            ret_val = ERROR;
        }
    } while (ch1 != EOF && ch2 != EOF);

	if(should_close_files)
	{
		fclose(file1);
		fclose(file2);
	}
	ret_val = ret_val == SUCCESS &&  (ch1 == EOF && ch2 == EOF)? SUCCESS : ERROR;
    return ret_val;
}

int count_commas(const char *line)
{
    int count = 0;
    /* Iterate through each character in the line */
    while (*line != NULL_TERMINATOR)
	{
        /*  If the character is a comma, increment the count */
        if (*line == ',') {
            count++;
        }
        /* Move to the next character */
        line++;
    }
    return count;
}


int skip_whitespace(char **line, int line_count ,StringVector *error_founds)
{
	char log_error[MAX_LINE_SIZE];
	int index = 0;
	for (; isspace((*line)[index]); index++)
	{
		if ((*line)[index] == NEWLINE_CHARACTER)
		{
			sprintf(log_error, "Invalid syntax. found in line: %d", line_count);
			if (string_vector_push_back(error_founds, log_error) == ERR) 
			{ 
				LOG(ERROR, PARAMS, "string_vector_push_back failed"); 
				return ERR; 
			} 
			LOG(ERROR, PARAMS, log_error); 
			return ERR; 
		}
	}

	*line += index;
	return SUCCESS;
}


int is_line_too_long(char *line, int line_count, StringVector *warning_founds)
{
	char log_warning[MAX_LINE_SIZE];
	int i = 0;
	
	/* check if the line is too long */
	for (i = 0; i < MAX_LINE_SIZE; i++)
	{
		if (line[i] == NEWLINE_CHARACTER)
		{
			break;
		}
	}

	if (i == MAX_LINE_SIZE)
	{
		sprintf(log_warning, "line: %d is longer than allowed", line_count);
		PUSH_WARNING(log_warning, warning_founds)
	}

	return SUCCESS;
}
