/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file logger.c
 * @brief  This file contains implementation of the logger module.
 * 
 *********************************************************************/

#include "logger.h"

#define LOG_FILE "logfile.txt"

void LOG(LogLevel level, const char *filename, int line_number, const char *format, ...) {
    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[20];
    const char *level_str;
    FILE *file = fopen(LOG_FILE, "w");
	/*FILE *file = fopen(LOG_FILE, "a");*/
    va_list args;

    if (file == NULL) {
        perror("Error opening log file");
        return;
    }

    /* Get current time */

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    /* Format timestamp */

    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    /* Format log level */

    switch (level) {
        case DEBUG:
            level_str = "DEBUG";
            break;
        case INFO:
            level_str = "INFO";
            break;
        case WARNING:
            level_str = "WARNING";
            break;
        case ERROR:
            level_str = "ERROR";
            break;
        default:
            level_str = "UNKNOWN";
    }

    /* Format log message */
    va_start(args, format);
    fprintf(file, "[%s] [%s:%d] [%s] ", timestamp, filename, line_number, level_str);
    vfprintf(file, format, args);
    fprintf(file, "\n");
    va_end(args);

    fclose(file);
}
