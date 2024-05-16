/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file ext_ent_file_builder.h
 * @brief This file contains the implementation of the extern/entry output files builder.
 * 
 *********************************************************************/

#ifndef EXT_ENT_FILE_BUILDER_H
#define EXT_ENT_FILE_BUILDER_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "../assembler/assembler.h"


/**
 * Builds an extern output file for assembler process.
 *
 * This function takes a filename and an assembler data structure as input and
 * builds an extern file based on the provided data. The extern
 * file contains information about external symbols used in the assembly process.
 *
 * @param filename The name of the external entity file to be created.
 * @param assembler A pointer to the assembler data structure.
 * @return Returns 0 if the external entity file is successfully built, otherwise returns -1.
 */
int build_extern_file(const char *filename, assembler_data_t *assembler);

/**
 * Builds an entry file using the specified filename and assembler data.
 *
 * @param filename The name of the file to be built.
 * @param assembler A pointer to the assembler data.
 * @return An integer indicating the success or failure of the operation.
 */
int build_entry_file(const char *filename, assembler_data_t *assembler);


#endif /* EXT_ENT_FILE_BUILDER_H */





