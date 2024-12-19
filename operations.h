/**************************************************************
 *
 *                     operations.h
 *
 *     Assignment: HW 6: um
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 04/11/24
 *
 *     Summary: Function declarations for the operations that can be
 *             executed by the Universal Machine. The functions in this
 *             file are used to execute the instructions from the program
 *             file passed in as an argument to the Universal Machine.
 * 
 **************************************************************/

#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "segment.h"

/*****************************************************************
 *                  Arithmetic Function Declarations
 *****************************************************************/
extern void cmov(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c);
extern void add(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c);
extern void multiply(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c);
extern void divide(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c);
extern void nand(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c);

/*****************************************************************
 *                  Control Function Declarations
 *****************************************************************/
extern void seg_load(Address_space space, uint32_t *regs, uint32_t a, 
                     uint32_t b, uint32_t c);
extern void seg_store(Address_space space, uint32_t *regs, uint32_t a, 
                      uint32_t b, uint32_t c);
extern void halt(Address_space space);

/*****************************************************************
 *                  I/O Function Declarations
 *****************************************************************/
extern void output(uint32_t *regs, uint32_t c);
extern void input(uint32_t *regs, uint32_t c);

/*****************************************************************
 *                  Segment Function Declarations
 *****************************************************************/
extern void load_program(Address_space space, uint32_t *regs, uint32_t b, 
                         uint32_t c, size_t *prog_counter, size_t *num_inst);
extern void load_value(uint32_t *regs, uint32_t a, uint32_t val);

#endif