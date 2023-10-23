/* block_array.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the functions to create an array of 
 *           each 2 by 2 pixel block while retaining the correct information.
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
#include "compression.h"
#include "math.h"

#define denom 4.0
#define block_size 2
#define max_val 0.3

unsigned average_Pb(A2Methods_UArray2 uarray, A2Methods_T methods, 
        int col, int row); 
unsigned average_Pr(A2Methods_UArray2 uarray, A2Methods_T methods,
         int col, int row);
void create_blocks(A2Methods_UArray2 uarray, A2Methods_T methods, 
        int col, int row, 
unsigned Pb_chroma, unsigned Pr_chroma, float a, signed b, signed c, signed d);
void cosine_transform(A2Methods_UArray2 uarray, A2Methods_T methods, int col, 
int row, unsigned Pb_chroma, unsigned Pr_chroma, A2Methods_UArray2 block_array, 
        A2Methods_T plain_methods);
signed bitScaled(float value);

/*
* name:      to_block  
* purpose:   
* arguments: 
* returns:    
* effects:  
*/
A2Methods_UArray2 to_block(A2Methods_UArray2 uarray, A2Methods_T methods,
        A2Methods_T plain_methods)
{
        unsigned Pb_chroma = 0;
        unsigned Pr_chroma = 0;

        /* width and height divided by 2 to account for the pixels in each
        block */
        int uarray2_width = methods->width(uarray) / block_size;
        int uarray2_height = methods->height(uarray) / block_size;

        /* make new array where each cell represents one block */
        A2Methods_UArray2 block_info = plain_methods->new(uarray2_width,
                uarray2_height, sizeof(struct block));

        int width = methods->width(uarray);
        int height = methods->height(uarray);

        /* access top left pixel of each block */
        for (int row = 0; row < height; row = row + block_size) {
                for (int col = 0; col < width; col = col + block_size) {
                        /* get average chroma values from each 2 by 2 block */
                        Pb_chroma = average_Pb(uarray, methods, col, row);
                        Pr_chroma = average_Pr(uarray, methods, col, row);

                        /* calculate cosine transform equations */
                        cosine_transform(uarray, methods, col, row, Pb_chroma, 
                                Pr_chroma, block_info, plain_methods);
                }
        }

        /* free blocked uarray */
        methods->free(&uarray);
        return block_info;
}

/*
* name:      cosine_transform   
* purpose:   calculate the discrete cosine transform for each 2 by 2 pixel
*            block
* arguments: the blocked UArray2, the methods being used, the current column,
*            the current row, the average Pb and Pr chroma values, the UArray2
*            of blocks, and the plain methods
* returns:   none
* effects:   populates UArray2 of blocks
*/
void cosine_transform(A2Methods_UArray2 uarray, A2Methods_T methods, 
        int col, int row, unsigned Pb_chroma, unsigned Pr_chroma, 
        A2Methods_UArray2 block_array, A2Methods_T plain_methods) 
{
        /* get each video_color element at each spot in the block*/
        video_color num1 = methods->at(uarray, col, row); 
        video_color num2 = methods->at(uarray, col + 1, row); 
        video_color num3 = methods->at(uarray, col, row + 1); 
        video_color num4 = methods->at(uarray, col + 1, row + 1); 
        
        /* set Y values to floats */
        float Y1 = num1->Y;
        float Y2 = num2->Y;
        float Y3 = num3->Y;
        float Y4 = num4->Y;



        /* calculate the discrete cosine transform */
        float a = (Y4 + Y3 + Y2 + Y1) / denom;
        float b = (Y4 + Y3 - Y2 - Y1) / denom;
        float c = (Y4 - Y3 + Y2 - Y1) / denom;
        float d = (Y4 - Y3 - Y2 + Y1) / denom;     

        /* scale the b, c, and d values */
        signed new_b = bitScaled(b);
        signed new_c = bitScaled(c);
        signed new_d = bitScaled(d);
               
        /* create the block being stored in the UArray2 of blocks */
        create_blocks(block_array, plain_methods, col, row, Pb_chroma,
                Pr_chroma, a, new_b, new_c, new_d);
}

/*
* name:      average_Pb
* purpose:   calculate the average Pb value
* arguments: blocked UArray2 of pixels, the methods being used, the current
*            column and row
* returns:   unsigned integer of the average Pb value
* effects:   none
*/
unsigned average_Pb(A2Methods_UArray2 uarray, A2Methods_T methods,
        int col, int row) 
{
        float sum = 0;

        /* loop through the block to get each Pb value */
        for (int c = col; c < col + block_size; c++) { 
                for (int r = row; r < row + block_size; r++) {
                        /* add each Pb value */
                        video_color num = methods->at(uarray, c, r); 
                        sum = sum + num->Pb;
                }
        }

        /* take the average Pb value */
        sum = sum / denom;

        /* convert to four-bit value */
        unsigned chroma = Arith40_index_of_chroma(sum);

        return chroma;
}

/*
* name:      average_Pr
* purpose:   calculate the average Pr value
* arguments: blocked UArray2 of pixels, the methods being used, the current
*            column and row
* returns:   unsigned integer of the average Pr value
* effects:   none
*/
unsigned average_Pr(A2Methods_UArray2 uarray, A2Methods_T methods,
        int col, int row) 
{
        float sum = 0;

         /* loop through the block to get each Pr value */
        for (int c = col; c < col + block_size; c++) { 
                for (int r = row; r < row + block_size; r++) {
                        /* add each Pr value */
                        video_color num = methods->at(uarray, c, r); 
                        sum = sum + num->Pr;
                }
        }
        
        /* take the average Pr value */
        sum = sum / denom;
        
        /* convert to four-bit value */
        unsigned chroma = Arith40_index_of_chroma(sum);

        return chroma;
}

/*
* name:      create_blocks
* purpose:   populate members of each block 
* arguments: blocked UArray2 of pixels, the methods being used, the current
*            column, the current row, the Pb and Pr chroma values, the signed
*            a, b, c, and d values
* returns:   none
* effects:   frees necessary memory allocated
*/
void create_blocks(A2Methods_UArray2 uarray, A2Methods_T methods, int col, 
        int row, unsigned Pb_chroma, unsigned Pr_chroma, float a, signed b,
        signed c, signed d)
{
        int new_col = col / block_size;
        int new_row = row / block_size;

        /* get elem at current column and row */
        block elem = methods->at(uarray, new_col, new_row); 
        
        /* malloc for new block */
        block new_block = malloc(sizeof(struct block));
        assert(new_block != NULL);

        /* set members of new block*/
        new_block->chroma_pr = Pr_chroma;
        new_block->chroma_pb = Pb_chroma;
        new_block->a = a;
        new_block->b = b;
        new_block->c = c;
        new_block->d = d;
        
        /* store new block at elem */
        *elem = *new_block;

        /* free new block */
        free(new_block);
}

/*
* name:      bitScaled   
* purpose:   scale the floats 
* arguments: the value to be scaled
* returns:   the new scaled value
* effects:   none
*/
signed bitScaled(float value)
{
        signed new_val;

        /* if value is in range assign to value between -15 and 15 */
        if ((value <= max_val) && (value >= -(max_val))) {
                new_val = value * 103.333;
        } else if (value > max_val) {
                /* if value is greater, assign to 15 */
                new_val = 31;
        } else if (value < max_val) {
                /* if value is lesser, assign to -15 */
                new_val = -31;
        }

        return new_val;
}