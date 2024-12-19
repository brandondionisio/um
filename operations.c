/**************************************************************
 *
 *                     operations.c
 *
 *     Assignment: HW 6: um
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 04/11/24
 *
 *     Summary: Implementation of the arithmetic and control operations that 
 *              are executed by the Universal Machine. These functions perform
 *              operations that are specified by the opcode of instructions
 *              read in from a program file and execute changes to the state of
 *              registers and the address space of the machine accordingly.
 * 
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "seq.h"
#include "uarray.h"
#include "operations.h"

/********** Address_space ********
 * 
 * Struct to hold all the information needed to manage the segments in the
 * address space, including the sequence of segments in use, the number of
 * segments, and the sequence of unmapped segments.
 *
 *******************/
struct Address_space {
        Seq_T in_use; /* Seq_T of all segments that contain UArrays of words */
        Seq_T unmapped; /* Seq_T of all of the unmapped segments */
};

/* Constant for the maximum value of a 32-bit word */
#define NUM_MAX 4294967296

/* Constant for the maximum value of an ASCII character */
#define MAX_ASCII 256

/****************** cmov *******************
 * 
 * Conditionally moves the value in register b to register a if the value in 
 * register c is not 0.
 *
 * Parameters:
 *        uint32_t *regs:  pointer to an array of 8 32-bit registers
 *        uint32_t a:      register to move value to
 *        uint32_t b:      register to move value from
 *        uint32_t c:      register to check if value is not 0
 * Returns:
 *        None.
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a, b, c are valid register numbers (0-7).
 * 
 * Notes: 
 *      If the value in register c is not 0, the value in register b is moved
 *      to register a.
 *      If the value in register c is 0, nothing happens.
 *
 ********************************************/
extern void cmov(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c)
{
        /* Move the value in register b to register a if the value in register
         * c is not 0 */
        if (regs[c] != 0) {
                regs[a] = regs[b];
        }
}

/****************** add *******************
 * 
 * Adds the value in register b to the value in register c and stores the 
 * result in register a.
 *
 * Parameters:
 *        uint32_t *regs:  pointer to an array of 8 32-bit registers
 *        uint32_t a:      register to store the result
 *        uint32_t b:      register containing the first value
 *        uint32_t c:      register containing the second value
 * Returns:
 *        None.
 *      
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a, b, c are valid register numbers (0-7).
 * 
 * Notes: 
 *      The value in register a is set to the sum of the values in register b
 *      and register c. The result is modded by NUM_MAX to ensure it fits in a
 *      32-bit word.
 *
 ********************************************/
extern void add(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c)
{
        /* Add the value in register b to the value in register c and store the
         * result in register a. Mod the result by NUM_MAX to ensure it fits in
         * a 32-bit word */
        regs[a] = (regs[b] + regs[c]) % NUM_MAX;
}

/****************** multiply *******************
 * 
 * Multiplies the value in register b by the value in register c and stores the
 * result in register a.
 *
 * Parameters:
 *        uint32_t *regs:  pointer to an array of 8 32-bit registers
 *        uint32_t a:      register to store the result
 *        uint32_t b:      register containing the first value
 *        uint32_t c:      register containing the second value
 * Returns:
 *        None.
 *      
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a, b, c are valid register numbers (0-7).
 * 
 * Notes: 
 *      The value in register a is set to the product of the values in register
 *      b and register c. The result is modded by NUM_MAX to ensure it fits in
 *      a 32-bit word.
 *
 ********************************************/
extern void multiply(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c)
{
        /* Multiply the value in register b by the value in register c and 
         * store the result in register a */
        regs[a] = (regs[b] * regs[c]) % NUM_MAX;
}

/****************** divide *******************
 * 
 * Divides the value in register b by the value in register c and stores the
 * result in register a.
 *
 * Parameters:
 *        uint32_t *regs:  pointer to an array of 8 32-bit registers
 *        uint32_t a:      register to store the result
 *        uint32_t b:      register containing the dividend
 *        uint32_t c:      register containing the divisor
 * Returns:
 *        None.
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a, b, c are valid register numbers (0-7).
 *      The value in register c is not 0. If it is, a CRE is raised.
 * Notes: 
 *      The value in register a is set to the result of the division of the 
 *      value in register b by the value in register c.
 *
 ********************************************/
extern void divide(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c)
{
        /* Check if register c is not 0 */
        assert(regs[c] != 0);

        /* Divide the value in register b by the value in register c and store
         * the result in register a */
        regs[a] = regs[b] / regs[c];
}

/****************** nand *******************
 * 
 * Performs a bitwise NAND operation on the values in register b and register c
 * and stores the result in register a.
 *
 * Parameters:
 *        uint32_t *regs:  pointer to an array of 8 32-bit registers
 *        uint32_t a:      register to store the result
 *        uint32_t b:      register containing the first value
 *        uint32_t c:      register containing the second value
 * Returns:
 *      None.  
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a, b, c are valid register numbers (0-7).
 * Notes: 
 *      The value in register a is set to the result of the bitwise NAND 
 *      operation on the values in register b and register c.
 *
 ********************************************/
extern void nand(uint32_t *regs, uint32_t a, uint32_t b, uint32_t c)
{
        /* Perform bitwise NAND operation on registers b and c and store the 
         * result in register a */
        regs[a] = ~(regs[b] & regs[c]);
}

/****************** seg_load *******************
 * 
 * Loads the value from the word at the segment of register b with the offset 
 * of register c into register a.
 *
 * Parameters:
 *        Address_space space:  pointer to an address space struct
 *        uint32_t *regs:       pointer to an array of 8 32-bit registers
 *        uint32_t a:           register to store the value
 *        uint32_t b:           register containing the address
 *        uint32_t c:           register containing the offset
 * Returns:
 *        None.
 * Expects:
 *      space is not NULL and is a valid pointer to an address space struct.
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a, b, c are valid register numbers (0-7).
 * Notes: 
 *      The value in register a is set to the value in the word in the segment
 *      of register b and the offset of register c.
 *
 ********************************************/
extern void seg_load(Address_space space, uint32_t *regs, uint32_t a, 
                     uint32_t b, uint32_t c)
{
        /* Set the value in register a to the value of the word at the segment
         * in register b and the offset in register c */
        regs[a] = *(uint32_t *)word_at(space, regs[b], regs[c]);
}

/****************** seg_store *******************
 * 
 * Stores the value in register c to the word at the segment of register a with
 * the offset of register b.
 *
 * Parameters:
 *        Address_space space:  pointer to an address space struct
 *        uint32_t *regs:       pointer to an array of 8 32-bit registers
 *        uint32_t a:           register containing the address
 *        uint32_t b:           register containing the offset
 *        uint32_t c:           register containing the value to store
 * Returns:
 *        None.
 * Expects:
 *      space is not NULL and is a valid pointer to an address space struct.
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a, b, c are valid register numbers (0-7).
 * Notes: 
 *      The value in register c is stored in the word at the segment in 
 *      register a and the offset in register b.
 *
 ********************************************/
extern void seg_store(Address_space space, uint32_t *regs, uint32_t a, 
                      uint32_t b, uint32_t c)
{
        /* Get the word at the address in register a and the offset of 
         * register b and store value in register c to that word location */
        uint32_t *word = word_at(space, regs[a], regs[b]);
        *word = regs[c];
}

/****************** halt *******************
 * 
 * Ends the program by freeing all the segments in the address space and 
 * exiting the program.
 *
 * Parameters:
 *       Address_space space: pointer to an address space struct
 * Returns:
 *       None.
 * Expects:
 *      space is not NULL and is a valid pointer to an address space struct.
 * Notes: 
 *      All the segments in the address space are freed and the program exits.
 *
 ********************************************/
extern void halt(Address_space space)
{
        /* Free all the segments in the address space */
        free_all_segments(space);

        /* Exit the program */
        exit(0);
}

/****************** output *******************
 * 
 * Outputs the character in register c to stdout.
 *
 * Parameters:
 *      uint32_t *regs:  pointer to an array of 8 32-bit registers
 *      uint32_t c:      register containing the character to output
 * Returns:
 *      None.
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      c is a valid register number (0-7).
 *      The value in register c is a valid ASCII character. If it is not, a CRE
 *      is raised.
 * Notes: 
 *      The character in register c is output to stdout.
 *
 ********************************************/
extern void output(uint32_t *regs, uint32_t c)
{
        /* Check if register c is valid */
        assert(regs[c] < MAX_ASCII);

        /* Output the character in register c */
        printf("%c", regs[c]);
}

/****************** input *******************
 * 
 * Gets input from stdin and stores it in register c. If EOF is reached, the
 * value in register c is set to a 32-bit word in which every bit is 1.
 *
 * Parameters:
 *        uint32_t *regs:  pointer to an array of 8 32-bit registers
 *        uint32_t c:      register to store the input
 * Returns:
 *        None.
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      c is a valid register number (0-7).
 *      The value inputted is a valid ASCII character. If it is not, a CRE is
 *      raised.
 * Notes: 
 *      The value in register c is set to the input from stdin. If EOF is 
 *      reached, the value in register c is set to a 32-bit word in which every
 *      bit is 1.
 *
 ********************************************/
extern void input(uint32_t *regs, uint32_t c)
{
        /* Get input from stdin */
        int input = getchar();

        /* Check if input is valid */
        assert(input < MAX_ASCII);

        /* If EOF, set input to 32-bit word in which every bit is 1 */
        if (input == EOF) {
                input = 0xFFFFFFFF;
        }
        
        /* Store input in register */
        regs[c] = input;
}

/****************** load_program *******************
 * 
 * Loads the program from the segment in register b into the 0 segment if the
 * value in register b is not 0. The number of instructions is set to the 
 * length of the segment in the 0 segment and, in all cases, the program 
 * counter is set to the value in register c. 
 *
 * Parameters:
 *        Address_space space:  pointer to an address space struct
 *        uint32_t *regs:       pointer to an array of 8 32-bit registers
 *        uint32_t b:           register containing the segment to load
 *        uint32_t c:           register containing the program counter
 *        size_t *prog_counter: pointer to the program counter
 *        size_t *num_inst:     pointer to the number of instructions
 * Returns:
 *        None.
 * Expects:
 *      space is not NULL and is a valid pointer to an address space struct.
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      b, c are valid register numbers (0-7).
 *      prog_counter and num_inst are not NULL.
 * Notes: 
 *      If the value in register b is not 0, segment 0 is freed abandoned by 
 *      being freed and the program in the segment in register b is loaded into
 *      the 0 segment. The program counter is set to the value in register c 
 *      and the number of instructions is set to the length of the segment in 
 *      the 0 segment.
 *
 ********************************************/
extern void load_program(Address_space space, uint32_t *regs, uint32_t b, 
                         uint32_t c, size_t *prog_counter, size_t *num_inst)
{
        /* Check if register b is not 0 */
        if (regs[b] != 0) {
                /* Free segment 0 */
                free_segment(space, 0);

                /* Fetch the length of the segment being duplicated */
                int len = UArray_length(Seq_get(space->in_use, regs[b]));

                /* Create a new segment of the length of the segment being 
                 * duplicated with each element being the size of a 32-bit
                 * word */
                UArray_T new_seg = UArray_new(len, sizeof(uint32_t));

                /* Copy the contents of the segment being duplicated to the
                 * newly created segment */
                for (int seg_index = 0; seg_index < len; seg_index++) {
                        uint32_t *orig_word = word_at(space, regs[b],
                                                                    seg_index);
                        uint32_t *copied_word = UArray_at(new_seg, seg_index);
                        *copied_word = *orig_word;
                }

                /* Add the newly duplicated segment to the position of segment 
                 * 0 in the sequence of segments */
                Seq_put(space->in_use, 0, new_seg);

                /* Update the number of instructions to the length of the 
                 * newly duplicated segment that is now in the 0 segment */
                *num_inst = (size_t)len;
        }
        /* Update the program counter to the value in register c */
        *prog_counter = (size_t)(regs[c]);
}

/****************** load_value *******************
 * 
 * Loads the value val into register a.
 *
 * Parameters:
 *        uint32_t *regs:  pointer to an array of 8 32-bit registers
 *        uint32_t a:      register to store the value
 *        uint32_t val:    value to store in register a
 * Returns:
 *        None.
 * Expects:
 *      uint32_t *regs is not NULL and is a valid pointer to an array of 8
 *      32-bit registers.   
 *      a is a valid register number (0-7).
 * Notes: 
 *      The value val is stored in register a.
 *
 ********************************************/
extern void load_value(uint32_t *regs, uint32_t a, uint32_t val)
{
        /* Store the value in register a */
        regs[a] = val;
}
