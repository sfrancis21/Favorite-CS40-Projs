/* compression.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the overview of the compression steps.
 *          It makes the appropriate calls to other files to compress the 
 *          image.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress40.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "trimmed_array.h"
#include "video_color_array.h"
#include "block_array.h"
#include "packing.h"

void print_codewords(int col, int row, A2Methods_UArray2 array2, 
        A2Methods_Object *elem, void *cl);
void print_final_array(A2Methods_UArray2 final_array, 
        A2Methods_T methods, int width, int height);

/*
* name:       compression 
* purpose:    compress given image
* arguments:  file of image to compress
* returns:    none
* effects:    calls necessary functions to carry out compression
*/
void compression(FILE *input) 
{
        /* set methods and create Pnm_ppm image of the given image */
        A2Methods_T methods = uarray2_methods_plain; 
        Pnm_ppm image = Pnm_ppmread(input, methods);

        /* call to function in trimmed_array.c to trim image to have even 
        width and height and set new width and height */
        Pnm_ppm new_image = trim(image);
        int width = new_image->width;
        int height = new_image->height;

        /* call to function in video_color_array.c to create new blocked array 
        of pixels */
        A2Methods_UArray2 color_space = to_color_space(new_image);

        /* reset methods to blocked */
        A2Methods_T blocked_methods = uarray2_methods_blocked;

        /* call to function in block_array.c to create an array of the 
        blocks */
        A2Methods_UArray2 block_info = to_block(color_space, 
                blocked_methods, methods);

        /* call to function in packing.c to create the codewords for
        each block in the array from the previous step */
        A2Methods_UArray2 final_array = packing_bits(block_info, methods);

        /* print out final array of codewords */
        print_final_array(final_array, methods, width, height);
}

/*
* name:      print_final_array 
* purpose:   print out the final array of codewords
* arguments: the final UArray2 of codewords, the methods being used, the width
*            of the original image, the height of the original image
* returns:   none
* effects:   frees memory associated with the final array of codewords
*/
void print_final_array(A2Methods_UArray2 final_array, A2Methods_T methods, 
        int width, int height)
{
        /* print header*/
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
        
        /* map and print each codeword */
        methods->map_row_major(final_array, print_codewords, NULL);

        /* free memory */
        methods->free(&final_array);
}

/*
* name:      print_codewords
* purpose:   apply function to print out each codeword
* arguments: the current column and row, the UArray3 being mapped, 
* returns:   none
* effects:   none
*/
void print_codewords(int col, int row, A2Methods_UArray2 array2, 
        A2Methods_Object *elem, void *cl) 
{
        (void)array2;
        (void)col;
        (void)row;
        (void)cl;

        uint64_t curr = *(uint64_t *)elem;
        int width = 8;        

        /* print each byte of each code word in big endian */
        for (int i = 24; i >= 0; i = i - 8) {
                uint64_t byte = Bitpack_getu(curr, width, i);
                putchar(byte);
        }
}