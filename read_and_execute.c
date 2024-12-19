
/**************************************************************
 *
 *                     read_and_execute.c
 *
 *     Assignment: HW 6: um
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 04/11/24
 *
 *     Summary: Implementation of the functions to read the instructions from
 *              an input file into the address space, execute the instructions
 *              in the address space appropriately by calling external  
 *              operations functions, and fully free the segments in the 
 *              address space after all instructions have been executed.
 * 
 **************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "read_and_execute.h"
#include "segment.h"
#include "bitpack.h"
#include "operations.h"

typedef uint32_t Um_instruction; /* private abbreviation */

/********** Um_opcode ********
 * 
 * Enum to hold all the possible opcodes for the instructions that can be 
 * executed by the Universal Machine.
 *
 *******************/
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

/****************** um_driver *******************
 * 
 * Function to call the appropriate functions to read the instructions from the
 * file into the address space, execute the instructions, and free the segments
 * in the address space. 
 *
 * Parameters:
 *            FILE *fp: pointer to the file that holds the instructions
 *     size_t num_inst: number of instructions in the file
 * Returns:
 *        None.
 * Expects:
 *      The file pointer is not NULL.
 *      The number of instructions is greater than 0.
 * Notes: 
 *      The function initializes 8 registers and sets each to 0. It then 
 *      creates a new address space, reads the instructions from the file into
 *      the address space, executes each instruction, and frees all the 
 *      segments in the address space.
 * 
 ********************************************/
extern void um_driver(FILE *fp, size_t num_inst) 
{
        /* Initialize 8 registers and set each to 0 */
        uint32_t registers[8] = { 0 };

        /* Create a new address space */
        Address_space space = new_address_space();

        /* Read instructions from file into address space */
        read_instructions(fp, space, num_inst);

        /* Execute each instructions */
        execute_instructions(space, num_inst, registers);

        /* Free all the segments in the address space */
        free_all_segments(space);
}

/*************** read_instructions ***************
 * 
 * Reads in the 32-bit words in the given file and places them as instructions
 * in the 0 segment of the given address space.
 *
 * Parameters:
 *      FILE *fp:            pointer to the file that holds the instructions
 *      Address_space space: an Address_space object in which the 0 segment
 *                           will be mapped.
 *      size_t num_inst:     number of instructions in the file
 * Returns:
 *      None.
 * Expects:
 *      None
 * Notes: 
 *      The function first initializes the 0 segment with the known length.
 *      It procedes to get each 32-bit instruction with getc and bitpacking.
 *      Finally, it populates the zero segment and closes the file upon
 *      finishing.
 * 
 ********************************************/
extern void read_instructions(FILE *fp, Address_space space, size_t num_inst) 
{
        /* Initialize variables to hold instruction and character inputted */
        Um_instruction word;
        int c;

        /* Create a new segment in the file space (0 segment) */
        map_segment(space, NULL, 0, 0, num_inst, true);

        /* Read the first character from the file */
        c = getc(fp);

        /* Read the intstructions from the file into 0 segment */
        for (size_t inst_count = 0; inst_count < num_inst; inst_count++) {
                /* Initialize word to 0 to start */
                word = 0;

                /* Read in the 32-bit word from the input file */
                for (int bit = sizeof(Um_instruction) - 1; bit >= 0; bit--) {
                        /* Add the character to the word */
                        word = (Um_instruction)Bitpack_newu((uint64_t)word, 8,
                                                       (bit * 8), (uint64_t)c);

                        /* Read the next character from the file */
                        c = getc(fp);
                }

                /* Place the word in the 0 segment at the index of the 
                 * instruction the program is at */
                uint32_t *word_location = word_at(space, 0, inst_count);
                *word_location = word;
        }
        /* After reading in the instructions, close the file */
        fclose(fp);
}

/*************** execute_instructions ***************
 * 
 * Executes the instructions which are contained in the 0 segment of the
 * given address space.
 *
 * Parameters:
 *      Address_space space: an Address_space object in which the 0 segment
 *                           will be mapped.
 *      size_t num_inst:     number of instructions in the file
 *      uint32_t *registers: a pointer to the array of unsigned 32-bit integers
 *                           that contain registers 0 - 7.
 * Returns:
 *      None.
 * Expects:
 *      None
 * Notes: 
 *      The function first initializes the program counter to point to the
 *      first instruction in the 0 segment and then iterates through. For
 *      each instruction, the function gets the register indices and then
 *      calls a function corresponding to the instruction's opcode.
 * 
 ********************************************/
extern void execute_instructions(Address_space space, size_t num_inst, 
                                 uint32_t *registers)
{
        /* Initialize program counter */
        size_t prog_counter = 0;

        /* Initialize boolean to check if last instruction was a LOADP so that
         * the program does not increment new prog_counter at end of loop */
        bool last_loadp = false;

        /* Execute instructions until program counter reaches the end of the 
         * number of instructions there are */
        while (prog_counter < num_inst) {
                /* Get instruction from index of prog_counter from 0 segment */
                uint32_t *instruction = word_at(space, 0, prog_counter);

                /* Fetch register indices from instruction */
                uint32_t a_index = get_A(*instruction);
                uint32_t b_index = get_B(*instruction);
                uint32_t c_index = get_C(*instruction);
                uint32_t a_lv_index = get_A_lv(*instruction);
                uint32_t value = get_val(*instruction);

                /* Reset boolean to false */
                last_loadp = false;

                /* Execute instruction based on the opcode of instruction */
                switch(get_op(*instruction))
                {
                        case CMOV:
                                /* Call conditional move function */
                                cmov(registers, a_index, b_index, c_index);
                                break;

                        case SLOAD:
                                /* Call segment load function */
                                seg_load(space, registers, a_index, b_index,
                                         c_index);
                                break;

                        case SSTORE:
                                /* Call segment store function */
                                seg_store(space, registers, a_index, b_index,
                                          c_index);
                                break;

                        case ADD:
                                /* Call add function */
                                add(registers, a_index, b_index, c_index);
                                break;

                        case MUL:
                                /* Call multiply function */
                                multiply(registers, a_index, b_index, c_index);
                                break;

                        case DIV:
                                /* Call divide function */
                                divide(registers, a_index, b_index, c_index);
                                break;

                        case NAND:
                                /* Call nand function */
                                nand(registers, a_index, b_index, c_index);
                                break;

                        case HALT:
                                /* Call halt function */
                                halt(space);
                                break;

                        case MAP:
                                /* Call map segment function */
                                map_segment(space, registers, b_index,
                                            c_index, 0, false);
                                break;

                        case UNMAP:
                                /* Call unmap segment function */
                                unmap_segment(space, registers, c_index);
                                break;

                        case OUT:
                                /* Call output function */
                                output(registers, c_index);
                                break;

                        case IN:
                                /* Call input function */
                                input(registers, c_index);
                                break;

                        case LOADP:
                                /* Call load program function */
                                load_program(space, registers, b_index, 
                                            c_index, &prog_counter, &num_inst);

                                /* Set bool to true to indicate LOAP was 
                                 * last operation executed */
                                last_loadp = true;
                                break;

                        case LV:
                                /* Call load value function */
                                load_value(registers, a_lv_index, value);
                                break;
                        
                        default:
                                exit(EXIT_FAILURE);
                                break;
                }

                /* Increment program counter to next instruction as long as 
                 * last instruction was not LOADP */
                if (!last_loadp) {
                        prog_counter++;
                }
        }
}

/****************** get_op *******************
 * 
 * Function to get the opcode from the instruction.
 *
 * Parameters:
 *      Um_instruction instruction: instruction to get the opcode from
 * Returns:
 *      An integer representing the opcode of the instruction
 * Expects:
 *      None
 * 
 ********************************************/
extern int get_op(Um_instruction instruction)
{
        return Bitpack_getu(instruction, 4, 28);
}

/****************** get_A *******************
 * 
 * Function to get the A register from the instruction, which is the 3 bits
 * starting at index 6 of a 32-bit instruction.
 *
 * Parameters:
 *      Um_instruction instruction: instruction to get register A from
 * Returns:
 *      An integer representing register A of the instruction
 * Expects:
 *      None
 * 
 ********************************************/
extern int get_A(Um_instruction instruction)
{
        return Bitpack_getu(instruction, 3, 6);
}

/****************** get_A_lv *******************
 * 
 * Function to get the A register from the a opcode 13 (load value)
 * instruction, which is the the three bits immediately less significant
 * than the 4 bit opcode that is the most significant 4 bits of the
 * instruction. 
 *
 * Parameters:
 *      Um_instruction instruction: instruction to get the A register from
 * Returns:
 *      An integer representing register A of the instruction
 * Expects:
 *      None
 * 
 ********************************************/
extern int get_A_lv(Um_instruction instruction)
{
        return Bitpack_getu(instruction, 3, 25);
}

/****************** get_B *******************
 * 
 * Function to get the B register from the instruction, which is the 3 bits
 * starting at index 3 of a 32-bit instruction.
 *
 * Parameters:
 *      Um_instruction instruction: the instruction to get the B register from
 * Returns:
 *      An integer representing register B of the instruction
 * Expects:
 *      None
 * 
 ********************************************/
extern int get_B(Um_instruction instruction)
{
        return Bitpack_getu(instruction, 3, 3);
}

/****************** get_C *******************
 * 
 * Function to get the C register from the instruction, which is the 3 bits
 * starting at index 0 of a 32-bit instruction.
 *
 * Parameters:
 *      Um_instruction instruction: the instruction to get the C register from
 * Returns:
 *      An integer representing register C of the instruction
 * Expects:
 *      None
 * 
 ********************************************/
extern int get_C(Um_instruction instruction)
{
        return Bitpack_getu(instruction, 3, 0);
}

/****************** get_val *******************
 * 
 * Function to get the value from the opcode 13 (load value) instruction, 
 * which is the 25 bits starting at index 0 of a 32-bit instruction.
 *
 * Parameters:
 *      Um_instruction instruction: the instruction to get the value from
 * Returns:
 *      An integer representing the value of the load value instruction
 * Expects:
 *      None
 * 
 ********************************************/
extern int get_val(Um_instruction instruction)
{
        return Bitpack_getu(instruction, 25, 0);
}
