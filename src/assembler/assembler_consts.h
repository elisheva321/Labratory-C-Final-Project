/* ******************************************************************* */
/* * Authors: Daniel Oren, Elisheva Cirota 
 *
 * @file assembler_consts.h
 * @brief  This file contains all common consts for assembler module.
 *********************************************************************/

#ifndef ASSEMBLER_CONST_H
#define ASSEMBLER_CONST_H

/* This file contains all common consts for assembler module */
#define DIR_LINE_PREFIX ('.')
#define LINE_PREFIX ('.')
#define DIR_LINE_DATA ("data")
#define DIR_LINE_STRING ("string")
#define DIR_LINE_ENTRY ("entry")
#define DIR_LINE_EXTERN ("extern")
#define CONST_DEFINE_LINE (".define")
#define INTEGER_OPERAND_PREFIX ('#')
#define STRING_PREFIX_OR_POSTFIX ('"')
#define COMMENT_LINE_PREFIX (';')
#define LABEL_POSTFIX (':')
#define COMMA (',')

#define IC_INITIAL_VALUE (100)
#define MAX_RESERVED_SYMBOL_NAME (5)
#define FOUND (1)
#define NOT_FOUND (0)
#define VALID (0)
#define INVALID (-1)
#define END_OF_LINE (-1)
#define MAX_NUM_OPERAND (2)

#endif /* ASSEMBLER_CONST_H */
