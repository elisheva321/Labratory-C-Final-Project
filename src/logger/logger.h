/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file logger.h
 * @brief This file contains the declaration of the logger module.
 *
 * The logger module provides function for logging messages to a file or console.
 * It includes function for logging different types of messages, such as debug, info, warning, and error messages.
 * This module is part of the final_project and is used for logging purposes.
 * 
 *********************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

typedef enum {
    DEBUG,
    INFO,
    WARNING,
    ERROR
} LogLevel;


/**
 * @brief Logs a message with the specified log level, filename, line number, and format.
 *
 * This function is used to log messages with different log levels. It takes the log level, filename,
 * line number, and a format string as input. The format string can contain placeholders that will be
 * replaced with the corresponding values provided in the variable arguments.
 *
 * @param level The log level of the message.
 * @param filename The name of the file where the log message is being generated.
 * @param line_number The line number in the file where the log message is being generated.
 * @param format The format string for the log message.
 * @param ... Variable arguments to be replaced in the format string.
 *
 * @note This function supports variable arguments using the `...` syntax.
 */
void LOG(LogLevel level, const char *filename, int line_number, const char *format, ...);

#define PARAMS  __FILE__, __LINE__

#endif /* LOGGER_H */




