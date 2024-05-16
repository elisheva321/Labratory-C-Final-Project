/* *******************************************************************
 * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file general_const.c
 * @brief  This file contains the implementation of the general constants used in the project.
 * 
 *********************************************************************/

#include "general_const.h"

const char *INSTRUCTIONS_NAMES[NUM_INSTRUCTIONS] = {
    "mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec",
    "jmp", "bne", "red", "prn", "jsr", "rts", "hlt"};

const char *REGISTERS_NAMES[NUM_OF_REGISTERS] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

const char *DIRECTIVES_NAMES[NUM_OF_DIRECTIVES] = {
    "data", "string", "entry", "extern"};
