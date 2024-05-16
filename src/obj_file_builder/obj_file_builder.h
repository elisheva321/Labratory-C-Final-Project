/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file obj_file_builder.h
 * @brief This file contains functions declarations of obj file builder.
 * 
 *********************************************************************/

#ifndef OBJ_FILE_BUILDER_H
#define OBJ_FILE_BUILDER_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "../assembler/assembler.h"

/**
 * Builds an obj file from the given assembler data.
 *
 * @param filename The name of the obj file to be created.
 * @param assembler A pointer to the assembler data.
 * @return Returns 0 if the obj file is successfully built, otherwise returns an error code.
 */
int build_obj_file(const char *filename, assembler_data_t *assembler);


#endif /*  OBJ_FILE_BUILDER_H */


