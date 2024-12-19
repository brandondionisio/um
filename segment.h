/**************************************************************
 *
 *                     segment.h
 *
 *     Assignment: HW 6: um
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 04/11/24
 *
 *     Summary: Function declarations for the Address_space ADT. The functions
 *              in this file are used to create a new instance of an address
 *              space, map and unmap segments, get a pointer to a word in the
 *              address space, and free associated memory.
 * 
 **************************************************************/

#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>
#include <stdbool.h>
#include "seq.h"

/*****************************************************************
 *                  Address_space Declaration
 *****************************************************************/
typedef struct Address_space *Address_space;

/*****************************************************************
 *                  Function Declarations
 *****************************************************************/
extern Address_space new_address_space();
extern void map_segment(Address_space space, uint32_t *regs, uint32_t b,
                                         uint32_t c, int length, bool is_zero);
extern void unmap_segment(Address_space space, uint32_t *regs,
                                                             uint32_t c_index);
extern uint32_t *word_at(Address_space space, uint32_t ID,
                                                          uint32_t word_index);
extern void free_segment(Address_space space, uint32_t ID);
extern void free_all_segments(Address_space space);

#endif