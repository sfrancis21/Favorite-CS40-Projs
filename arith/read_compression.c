/* read_compression.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the functions to make sense of and start
 *          decompressing the compressed image passed in.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "compress40.h"
#include "bitpack.h"
#include "arith40.h"
#include "pnm.h"
#include "uarray2.h"
#include "uarray2b.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "assert.h"
#include "decompression.h"
#include "read_compression.h"

void index_codewords(int col, int row, A2Methods_UArray2 array2, 
A2Methods_Object *elem, void *cl);

/*
* name:      read_compressed_image  
* purpose:   read compressed image file and retrieve information about original
*            image
* arguments: file of the compressed image
* returns:   a new Pnm_ppm image with the characteristics of the compressed 
*            image
* effects:   allocates space for the new Pnm_ppm image struct and its fields
*/
Pnm_ppm read_compressed_image(FILE *input)
{
        unsigned height, width;

        /* read in first line to get width and height */
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u", 
                &width, &height);
        
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        /* set methods */
        A2Methods_T methods = uarray2_methods_plain; 

        /* make new array of Pnm_rgb structs */
        int size = sizeof(struct Pnm_rgb);
        UArray2_T pixels_array = methods->new(width, height, size);        
        
        /* make new Pnm_ppm image */
        Pnm_ppm image = malloc(sizeof(struct Pnm_ppm));
        assert(image != NULL);

        /* set members of new image */
        image->width = width;
        image->height = height;
        image->pixels = pixels_array;
        image->denominator = 255;
        image->methods = methods;
        
        return image;
}

/*
* name:      get_codeword_array   
* purpose:   make a new UArray of the codewords 
* arguments: the input file, the new Pnm_ppm image
* returns:   a new UArray2 of the codewords
* effects:   none
*/
UArray2_T get_codeword_array(FILE *input, Pnm_ppm image)
{
        int size = sizeof(uint64_t);

        /* width and height are the numbers of pixels so we must divide
        each one by 2 to get the number of 2 by 2 blocks */
        int width = image->width / 2;
        int height = image->height / 2;

        /* new array of codewords*/
        UArray2_T codewords = image->methods->new(width, height, size);   

        /* map the new array to fill it with the codewords from the 
        input file */
        image->methods->map_row_major(codewords, index_codewords, input);

        return codewords;
}

/*
* name:      index_codewords      
* purpose:   apply function to receive each codeword 
* arguments: the current column and row, the UArray2 to be mapped, the current
*            element, and a void pointer to the closure
* returns:   none
* effects:   each element now contains the corresponding codeword
*/
void index_codewords(int col, int row, A2Methods_UArray2 array2, 
        A2Methods_Object *elem, void *cl)
{
        (void)col;
        (void)array2;
        (void)row;

        /* cast closure and elem */
        FILE *closure = (FILE *)cl;
        uint64_t *curr = (uint64_t *)elem;

        /* initialize new_elem */
        uint64_t new_elem = 0x0;
        int width = 8;

        /* receiving each byte in big endian */
        for (int i = 24; i >= 0; i = i - 8) {
                int byte = getc(closure);
                assert((byte != EOF) && (feof(closure) == 0));
                /* set elem to the bytes */
                new_elem = Bitpack_newu(new_elem, width, i, byte);
        } 
        /* set curr to new_elem */ 
        *curr = new_elem;
        //assert(*curr != NULL);
}