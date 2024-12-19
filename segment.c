/**************************************************************
 *
 *                     segment.c
 *
 *     Assignment: HW 6: um
 *        Authors: Dan Glorioso & Brandon Dionisio (dglori02 & bdioni01)
 *           Date: 04/11/24
 *
 *     Summary: Implementation of the functions for the Address_space ADT. 
 *              These functions allow the client to create a new instance
 *              of the object, map and unmap segments, get a pointer to a word
 *              in the address space, and free associated memory.
 * 
 **************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include "segment.h"
#include "seq.h"
#include "uarray.h"
#include "mem.h"
#include "assert.h"

/* Constant for the estimates number of element to create for the Seq_T */
#define HINT 0

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

/**************** new_address_space ****************
 * 
 * Creates a new instance of an Address_space object and declares its
 * attributed variables.
 *
 * Parameters:
 *      None
 * Returns:
 *      a pointer to the newly initialized Address_space object.
 * Expects:
 *      Expects the client to handle deallocating memory associated with the
 *      Address_space object.
 *      Allocation of the object is successful.
 *
 ********************************************/
extern Address_space new_address_space()
{
        /* Allocate memory for the Address_space struct */
        Address_space space;
        NEW(space);
        assert(space != NULL);

        /* Initalize the fields of the Address_space struct */
        space->in_use = Seq_new(HINT);
        space->unmapped = Seq_new(HINT);
        return space;
}

/**************** map_segment ****************
 * 
 * Maps a new segment with a number of words equal to the value in $r[C] or
 * length, depending on the value of is_zero. When mapping the segment,
 * each word is initialized to 0. If mapping the 0 segment, the bool is_zero
 * will be true and the number of instructions in the segment will be sent in
 * as the length. If not initally mapping the 0 segment, the length will be the
 * value in register c. A bit pattern that is not all zeroes and that does not
 * identify any currently mapped segment is placed in $r[B]. The new segment is
 * mapped as $m[$r[B]].
 * 
 * Parameters:
 *      Address_space space: an Address_space object in which the segment
 *                           will be mapped.
 *      uint32_t *regs:      a pointer to the array of registers 0-7.
 *      uint32_t b:          32-bit unsigned integer representing register b.
 *      uint32_t c:          32-bit unsigned integer representing register c.
 *      int length:          integer representing the length of the segment if 
 *                           mapping the 0 segment.
 *      bool is_zero:        boolean representing if mapping the 0 segment.
 * Returns:
 *      None
 * Expects:
 *      None
 *
 ********************************************/
extern void map_segment(Address_space space, uint32_t *regs, uint32_t b, 
                        uint32_t c, int length, bool is_zero)
{
        /* Get the length of the segment from register c */
        if (!is_zero) {
                length = regs[c];
        }

        /* Create a new UArray of words */
        UArray_T uarray = UArray_new(length, sizeof(uint32_t));
        
        /* Initalize all the words in the newly mapped segment to 0 */
        for (int i = 0; i < length; i++) {
                uint32_t *word = (uint32_t *)UArray_at(uarray, i);
                *word = 0;
        }

        /* Check for unmapped segment */
        if (Seq_length(space->unmapped) == 0) {
                /* There are no unmapped segments, so save the length of the
                 * sequence of segments (non-zero) to register b */
                if (!is_zero) {
                        regs[b] = (uint32_t)Seq_length(space->in_use);
                }

                /* Add UArray to the end of Seq_T of in_use segments */
                Seq_addhi(space->in_use, uarray);
        } else {
                /* Reuse unmapped segments for new segment by fetching the 
                 * index from the sequence of unmapped segments and removing it
                 * from the unmapped sequence */
                uint32_t unmap_index = 
                               (uint32_t)(uintptr_t)Seq_remlo(space->unmapped);

                /* Save the index of the unmapped segment to register b, which
                 * is not all zeros */
                regs[b] = unmap_index;

                /* Add the UArray to the index of first unmapped segment */
                Seq_put(space->in_use, unmap_index, uarray);
        }
}

/**************** unmap_segment ****************
 * 
 * Unmaps the segment $m[$r[C]] and adds the identifier $r[C] to the unmapped
 * segment sequence so that it can be mapped again.
 *
 * Parameters:
 *      Address_space space: an Address_space object from which we are
 *                           unmapping a segment.
 *      uint32_t *regs:      a pointer to the array of registers 0-7.
 *      uint32_t c:          32-bit unsigned integer representing register c.
 * Returns:
 *      None
 * Expects:
 *      $r[C] is not 0 (would be unmapping the 0 segment) if not, throw a CRE
 *
 ********************************************/
extern void unmap_segment(Address_space space, uint32_t *regs, 
                          uint32_t c_index)
{
        /* Get the ID of the segment to be unmapped */
        uint32_t ID = regs[c_index];

        /* CRE if ID is equal to 0 segment */
        assert(ID != 0);
        
        /* Free the segment at the given ID */
        free_segment(space, ID);

        Seq_put(space->in_use, ID, NULL);

        /* Add ID of the unmapped segment to the sequence of unmapped IDs */
        Seq_addlo(space->unmapped, (void *)(uintptr_t)ID);
}

/**************** word_at ****************
 * 
 * Returns a pointer to the word at the given word_index from the segment at
 * given ID, which is obtained from the given address space.
 *
 * Parameters:
 *      Address_space space: an Address_space object from which we are getting
 *                           the word.
 *      uint32_t ID:         unsigned 32-bit integer representing the ID of
 *                           the segment from which we are getting the word.
 *      uint32_t c:          unsigned 32-bit integer representing the index
 *                           of the word inside its segment.
 * Returns:
 *      uint32_t pointer to the word
 * Expects:
 *      ID is less than the number of segments in the address space.
 *      If not, throws a CRE.
 *      The segment at the given ID is not NULL, meaning unmapped. If it is, 
 *      throws a CRE.
 *
 ********************************************/
extern uint32_t *word_at(Address_space space, uint32_t ID, uint32_t word_index)
{
        /* CRE if ID is greater than the number of segments */
        assert(ID < (uint32_t)Seq_length(space->in_use));

        /* CRE if the segment at the given ID is unmapped (NULL) */
        assert(Seq_get(space->in_use, ID) != NULL);

        /* Get the UArray at the given ID */
        UArray_T uarray = (UArray_T)Seq_get(space->in_use, ID);

        /* Get the word at the specified index from UArray */
        uint32_t *word_p = (uint32_t *)UArray_at(uarray, word_index);
        return word_p;
}

/**************** free_segment ****************
 * 
 * Frees the segment at the given ID from the given address space.
 *
 * Parameters:
 *      Address_space space: an Address_space object from which we are freeing
 *                           a segment.
 *      uint32_t ID:         unsigned 32-bit integer representing the ID of
 *                           the segment of which we are freeing.
 * Returns:
 *      None
 * Expects:
 *      The segment we are trying to free is not NULL (throws a CRE if not).
 *
 ********************************************/
extern void free_segment(Address_space space, uint32_t ID)
{
        /* Get the UArray at the given ID */
        UArray_T seg = (UArray_T)Seq_get(space->in_use, ID);
        
        /* Free the UArray if it is not NULL */
        if (seg != NULL) {
                UArray_free(&seg);
        }
}

/**************** free_all_segments ****************
 * 
 * Frees all the segments associated with the given address space.
 *
 * Parameters:
 *      Address_space space: an Address_space object from which we are freeing
 *                           all the segments.
 * Returns:
 *      None
 * Expects:
 *      None
 *
 ********************************************/
extern void free_all_segments(Address_space space)
{
        /* Free all of the UArrays in the address space */
        int length = Seq_length(space->in_use);
        for (int i = 0; i < length; i++) {
                free_segment(space, 0);
                Seq_remlo(space->in_use);
        }
        
        /* Free the address space */
        if (space->in_use != NULL) {
                Seq_free(&(space->in_use));
        }

        /* Free the sequence of unmapped IDs */
        if (space->unmapped != NULL) {
                Seq_free(&(space->unmapped));
        }

        /* Free the address space */
        if (space != NULL) {
                FREE(space);
        }
}
