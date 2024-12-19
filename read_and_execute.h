/**************************************************************
 *
 *                     read_and_execute.h
 *
 *     Assignment: HW 6: um
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 04/11/24
 *
 *     Summary: Function declarations for processes that read in and execute
 *              the instructions from the program file passed in as an argument
 *              to the Universal Machine. 
 * 
 **************************************************************/


#ifndef READ_AND_EXECUTE_H
#define READ_AND_EXECUTE_H

#include "segment.h"

/*****************************************************************
 *                  Program Function Declarations
 *****************************************************************/
extern void um_driver(FILE *fp, size_t num_inst);
extern void read_instructions(FILE *fp, Address_space space, size_t num_inst);
extern void execute_instructions(Address_space space, size_t num_inst,
                                                          uint32_t *registers);

/*****************************************************************
 *                  Getter Function Declarations
 *****************************************************************/
extern int get_op(uint32_t instruction);
extern int get_A(uint32_t instruction);
extern int get_A_lv(uint32_t instruction);
extern int get_B(uint32_t instruction);
extern int get_C(uint32_t instuction);
extern int get_val(uint32_t instruction);

#endif