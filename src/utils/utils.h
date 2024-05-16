/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file utils.h
 * @brief   This file contains utility functions implementations.
 * 
 *********************************************************************/
#ifndef UTILS_H
#define UTILS_H

#define UNUSED(x) ((void)(x))

#include "../string_vector/string_vector.h"

enum msg_type
{
    MSG_TYPE__ERROR,
    MSG_TYPE__WARNING
};

/** @brief Duplicates a string.
 *  Allocates memory, copies content, returns pointer. Frees memory later.
 *  @param src The source string to duplicate.
 *  @return Pointer to the duplicated string, or NULL on allocation failure.
 */
char *str_dup(const char *s);

/**
 * @brief Calculates the Two's complement representation of a short integer.
 *
 * This function calculates the Two's complement representation of a signed short integer (`short`).
 *
 * @param num The input signed short integer value.
 *
 * @return The Two's complement representation of the input `num` as a `short`.
 *         - If `num` is positive or zero, the function returns `num` itself.
 *         - If `num` is negative and its Two's complement representation overflows
 *           the representable range for a `short`, the function returns `SHRT_MIN`
 *           to indicate an error condition.
 *
 */
unsigned short twos_complement_short(short num);

/**
 * Sets a specific number of bits in a short integer to a given value.
 *
 * Modifies a short integer (`num`) by setting a specified number of bits
 * (determined by the binary representation of `value`) starting from a particular bit position
 * (`starting_bit`). Returns a new short integer with the modified bits.
 *
 * Parameters:
 *   - num: The short integer to modify.
 *   - value: The value (integer) to set to the specified bits.
 *   - num_bits: The number of bits to set (must be between 1 and 8).
 *   - starting_bit: The starting bit position (0-based) to modify (inclusive).
 *
 * Returns:
 *   - A new short integer with the specified number of bits set based on the binary representation of `value`.
 *     - If `num_bits` is less than 1 or greater than 8, the function raises a ValueError
 *       (implementation depends on your environment).
 *     - If the `value` cannot be represented in the specified `num_bits`, the function might exhibit
 *       undefined behavior.
 *     - If the `starting_bit` is out of range (less than 0 or greater than 7),
 *       the function might exhibit undefined behavior.
 */
short set_bits_short(short num, short value_to_set_at_specific_bits, short num_bits, short starting_bit);

/**
 * Converts a string to a short integer with error checking.
 *
 * @param str The input string to convert.
 * @param error Set to 1 on error (invalid input, out of range), 0 otherwise.
 * @return The converted value on success, 0 on error.
 *
 * Uses strtol to ensure validity and range. Checks for extra characters and
 * handles conversion errors by setting *error accordingly.
 */
short string_to_short(const char *str, int *error);

/**
 * @brief Generates the output file name by appending a postfix to the input file name.
 *
 * This function takes the input file name and appends a postfix to it to generate the output file name.
 *
 * @param input_file_name The name of the input file.
 * @param output_file_name The buffer to store the generated output file name.
 * @param postfix The postfix to append to the input file name.
 */
void get_output_file_name(const char *input_file_name, char *output_file_name, const char *postfix);

/**
 * Counts the number of digits in a given number.
 *
 * @param num The number for which to count the digits.
 * @return The number of digits in the given number.
 */
short count_digit(short num);

/**
 * Compares two files.
 *
 * This function compares the contents of two files and returns an integer value indicating
 * whether the files are equal or not.
 *
 * @param file1 Pointer to the first file to compare.
 * @param file2 Pointer to the second file to compare.
 * @return An integer value indicating the result of the comparison:
 *         - 0 if the files are equal.
 *         - a negative value if an error occurred while reading the files.
 *         - a positive value if the files are not equal.
 */
int compare_files(FILE *file1, FILE *file2, int should_close_files);

/**
 * Counts the number of commas in a given string.
 *
 * @param line The string to count commas in.
 * @return The number of commas found in the string.
 */
int count_commas(const char *line);

/**
 * Skips whitespace characters in a string.
 *
 * This function skips whitespace characters (space, tab, newline) in a string.
 *
 * @param line The string to skip whitespace characters in.
 * @param line_count The current line number.
 * @param error_founds The vector to store error messages in.
 * @return The number of characters skipped.
 */
int skip_whitespace(char **line, int line_count ,StringVector *error_founds);

/**
 * Checks if a line is longer than the allowed maximum length.
 *
 * This function checks if a line is longer than the allowed maximum length (81 characters).
 * If the line exceeds the maximum length, a warning message is added to the provided StringVector.
 *
 * @param line The line to check.
 * @param line_count The current line number.
 * @param warning_founds The vector to store warning messages in.
 * @return SUCCESS if the line length is within the allowed limit, otherwise returns an error code.
 */
int is_line_too_long(char *line, int line_count, StringVector *warning_founds);

#endif /* UTILS_H */
