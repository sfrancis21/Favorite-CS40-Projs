/* bitpack.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the implementation of the bitpacking functions.
 *              
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "bitpack.h"
#include <stdbool.h>
#include <stdint.h>
#include "except.h"
#include <math.h>

uint64_t unsigned_left(uint64_t word, unsigned shift);
int64_t signed_right(int64_t word, unsigned shift);
uint64_t unsigned_right(uint64_t word, unsigned shift);
int64_t signed_left(int64_t word, unsigned shift);
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
        uint64_t value);
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
        int64_t value);

extern Except_T Bitpack_Overflow;
Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
* name:      Bitpack_fitsu   
* purpose:   see if an unsigned integer will fit in the given width
* arguments: the integer to check and the number of bits (width)
* returns:   true if the integer fits and false if it doesn't
* effects:   none
*/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);

        /* get max value */
        unsigned max_val = pow(2, width) - 1;
        
        if (n > max_val) {
                return false;
        } else {
                return true;
        }
}

/*
* name:      Bitpack_fitss  
* purpose:   see if a signed integer will fit in the given width
* arguments: the integer to check and the number of bits (width)
* returns:   true if the integer fits and false if it doesn't
* effects:   none
*/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);

        /* get max val */
        signed max_val = pow(2, width - 1) - 1;
        signed min_val = pow(2, width - 1) * (-1);
        
        if (n > max_val || n < min_val) {
                return false;
        } else {
                return true;
        }
}

/*
* name:      Bitpack_getu     
* purpose:   get an unsigned series of bits based on the given width and lsb
* arguments: the word to retrieve from, the width of bits, and the
*            location of the least significant bit
* returns:   a uint64_t of the value retrieved
* effects:   none
*/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        /* if width is 0 return value */
        if (width == 0) {
                uint64_t val = 0;
                return val;
        }
        
        unsigned check = width + lsb;

        assert(check <= 64);
        assert(width <= 64);

        /* shift bits to retrieve desired value */
        unsigned left_shift = 64 - (width + lsb);
        word = unsigned_left(word, left_shift);
        word = unsigned_right(word, (left_shift + lsb));

        return word;
}

/*
* name:      Bitpack_gets     
* purpose:   get a signed series of bits based on the given width and lsb
* arguments: the word to retrieve from, the width of bits, and the
*            location of the least significant bit
* returns:   an int64_t of the value retrieved
* effects:   none
*/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        /* if width is 0 return value */
        if (width == 0) {
                int64_t val = 0;
                return val;
        }
        
        unsigned check = width + lsb;
        assert(check <= 64);

        assert(width <= 64);

        /* shift bits to retrieve desired value */
        unsigned left_shift = 64 - (width + lsb);
        int64_t new_word = signed_left(word, left_shift);
        new_word = signed_right(new_word, (left_shift + lsb));
        
        return new_word;
}

/*
* name:      Bitpack_newu     
* purpose:   insert a given unsigned value into the codeword
* arguments: a uint64_t of the codeword, the width of bits, the lsb, and the 
*            value to be added
* returns:   a uint64_t with the new value added
* effects:   none
*/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
        uint64_t value)
{
        unsigned check = width + lsb;
        assert(check <= 64);
        assert(width <= 64);

        /* check if value fits */
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* shift bits in order to find correct location to insert new value */
        uint64_t left_mask = ~0;
        left_mask = unsigned_left(left_mask, lsb + width);

        uint64_t right_mask = ~0;
        right_mask = unsigned_right(right_mask, 64 - lsb);

        uint64_t mask_compare;
        mask_compare = left_mask | right_mask;

        uint64_t empty_field = word & mask_compare;
        value = unsigned_left(value, lsb);

        uint64_t final_word = empty_field | value;

        return final_word;
}

/*
* name:      Bitpack_news     
* purpose:   insert a given signed value into the codeword
* arguments: a uint64_t of the codeword, the width of bits, the lsb, and the 
*            value to be added
* returns:   a uint64_t with the new value added
* effects:   none
*/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
        int64_t value)
{
        unsigned check = width + lsb;
        assert(check <= 64);
        assert(width <= 64);

         /* check if value fits */
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        
        uint64_t unsigned_value = value;
        unsigned_value = unsigned_left(unsigned_value, (64 - width));
        unsigned_value = unsigned_right(unsigned_value, 64 - width);

        /* shift bits in order to find correct location to insert new value */
        uint64_t left_mask = ~0;
        left_mask = unsigned_left(left_mask, lsb + width);

        uint64_t right_mask = ~0;
        right_mask = unsigned_right(right_mask, 64 - lsb);

        uint64_t mask_compare;
        mask_compare = left_mask | right_mask;

        uint64_t empty_field = word & mask_compare;
        unsigned_value = signed_left(unsigned_value, lsb);

        uint64_t final_word = empty_field | unsigned_value;

        return final_word;

}

/*
* name:      unsigned_left
* purpose:   shifts the uint64_t word left by shift number of bits
* arguments: the word to be shifted and the number of bits to shift by
* returns:   a new shifted uint64_t 
* effects:   none
*/
uint64_t unsigned_left(uint64_t word, unsigned shift) {
        assert(shift <= 64);
        
        if (shift == 64) {
                return 0;
        } else {
                uint64_t shifted_word = word << shift;
                return shifted_word;
        }
}

/*
* name:      signed_right
* purpose:   Shifts the int64_t word right by shift number of bits
* arguments: int64_t word (signed int to shift), unsinged shift(num 
*            of bits to shift the word)
* returns:   A int64_t word that has been shifted right or -1 if shifted 64 
*            bits
* effects:   None
*/
int64_t signed_right(int64_t word, unsigned shift) {
        assert(shift <= 64);
        
        if (shift == 64) {
                return -1;
        } else {
                int64_t shifted_word = word >> shift;
                return shifted_word;
        }
}

/*
* name:      unsigned_right
* purpose:   shifts the uint64_t word right by shift number of bits
* arguments: the word to be shifted and the number of bits to shift by
* returns:   a new shifted uint64_t 
* effects:   none
*/
uint64_t unsigned_right(uint64_t word, unsigned shift) {
        assert(shift <= 64);
        if (shift == 64) {
                return 0;
        } else {
                uint64_t shifted_word = word >> shift;
                return shifted_word;
        }
}

/*
* name:      signed_left
* purpose:   Shifts the int64_t word left by shift number of bits
* arguments: int64_t word (signed int to shift), unsinged shift(num 
*            of bits to shift the word)
* returns:   A int64_t word that has been shifted left or 0 if shifted 64 
*            bits
* effects:   None
*/
int64_t signed_left(int64_t word, unsigned shift) {
        assert(shift <= 64);
        if (shift == 64) {
                return 0;
        } else {
                int64_t shifted_word = word << shift;
                return shifted_word;
        }
}

