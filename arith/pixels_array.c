/* pixels_array.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the functions necessary to transform the
 *          array into an array of pixels.
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
#include "decompression.h"
#include "pixels_array.h"
#include "assert.h"

#define denom 255

typedef struct word_info *word_info;
struct word_info {
        uint64_t a;
        int64_t b;
        int64_t c;
        int64_t d;
        uint64_t pb;
        uint64_t pr;
};

void make_rgb(int col, int row, A2Methods_UArray2 array2, 
A2Methods_Object *elem, void *cl);
void get_bitfields(uint64_t curr, word_info word);
float unscaledBits(signed value);
void add_pixel(float Pb, float Pr, float Y1, float Y2, float Y3,  
float Y4, UArray2_T closure, int col, int row);
Pnm_rgb make_rgb_pixel(float Pb, float Pr, float Y);
Pnm_rgb create_pixel(float red, float green, float blue);


/*
* name:      float_to_rgb    
* purpose:   change each floating point value to an rgb pixel
* arguments: the Pnm_ppm image to fill, the UArraty2 of blocks, 
*            and the methods being used
* returns:   the populated Pnm_ppm image 
* effects:   none
*/
Pnm_ppm float_to_rgb(Pnm_ppm image, A2Methods_UArray2 array,
        A2Methods_T methods)
{
        /* map UArray2 to convert to pixels */
        methods->map_row_major(array, make_rgb, image->pixels);

        /* free UArray2 */
        methods->free(&array);

        return image;
}

/*
* name:      make_rgb 
* purpose:   apply function to turn codewords into rgb values
* arguments: the current column and row, the UArray2 of codewords being mapped,
*            the current element, and a void pointer to the closure
* returns:   none 
* effects:   populates the image pixels
*/
void make_rgb(int col, int row, A2Methods_UArray2 array2, 
A2Methods_Object *elem, void *cl)
{       
        (void)array2;
        UArray2_T closure = (UArray2_T)cl;
        uint64_t curr = *(uint64_t *)elem;

        /* malloc for word struct to contain each value from the code word */
        word_info word = malloc(sizeof(struct word_info));

        /* get each field from the codeword */
        get_bitfields(curr, word);

        /* convert 4 bit chroma codes back to average Pb and Pr values */
        float Pb = Arith40_chroma_of_index(word->pb);
        float Pr = Arith40_chroma_of_index(word->pr);

        /* convert a back to floating point */
        float a = (float)(word->a) / 63;

        /* unscaled b, c, and d */
        float b = unscaledBits(word->b);
        float c = unscaledBits(word->c);
        float d = unscaledBits(word->d);

        /* find Y values with a, b, c, d values */
        float Y1 = a - b - c + d; 
        float Y2 = a - b + c - d; 
        float Y3 = a + b - c - d;
        float Y4 = a + b + c + d;
        
        /* now working with an array of each pixel, not an array of each block
        so change the columns and rows back to their original values */
        col = col * 2;
        row = row * 2;

        /* add pixel to appropriate spot in image array of pixels */
        add_pixel(Pb, Pr, Y1, Y2, Y3, Y4, closure, col, row);

        /* free word struct */
        free(word);
}

/*
* name:      get_bitfields  
* purpose:   retrieve each bitfield from the given code word
* arguments: the current codeword and the struct to store each value
*            extracted in
* returns:   none
* effects:   sets each member in the word struct to its respective value
*/
void get_bitfields(uint64_t curr, word_info word)
{
        /* get each value and put in word struct */
        word->a = Bitpack_getu(curr, 6, 26);
        word->b = Bitpack_gets(curr, 6, 20);
        word->c = Bitpack_gets(curr, 6, 14);
        word->d = Bitpack_gets(curr, 6, 8);
        word->pb = Bitpack_getu(curr, 4, 4);
        word->pr = Bitpack_getu(curr, 4, 0);
}

/*
* name:      unscaledBits
* purpose:   unscale the given value
* arguments: the value to be unscaled
* returns:   a float of the new unscaled value
* effects:   none
*/
float unscaledBits(signed value)
{
        /* divide each value by 50 */
        float new_value = (float)value / 103.333;
        return new_value;
}

/*
* name:      add_pixel  
* purpose:   add each pixel to the right spot in the array of pixels
* arguments: floats of the average Pb and Pr values, floats of the Y1, Y2,
*            Y3, Y4 values, the UArray2 of image pixels, and the current 
*            column and row
* returns:   none
* effects:   each pixel in the pixel array is initialized and populated
*/
void add_pixel(float Pb, float Pr, float Y1, float Y2, float Y3,  
        float Y4, UArray2_T closure, int col, int row) 
{
        A2Methods_T methods = uarray2_methods_plain; 
        for (int i = 0; i < 4; i++) { 
                /* set each pixels values depending on which Y value it is*/
                if (i == 0) {
                        Pnm_rgb new_elem = make_rgb_pixel(Pb, Pr, Y1);
                        Pnm_rgb ptr = methods->at(closure, col, row); 
                        *ptr = *new_elem;
                        free(new_elem);
                } else if (i == 1) {
                        Pnm_rgb new_elem = make_rgb_pixel(Pb, Pr, Y2);
                        Pnm_rgb ptr = methods->at(closure, col + 1, row); 
                        *ptr = *new_elem;
                        free(new_elem);
                } else if (i == 2) {
                        Pnm_rgb new_elem = make_rgb_pixel(Pb, Pr, Y3);
                        Pnm_rgb ptr = methods->at(closure, col, row + 1); 
                        *ptr = *new_elem;
                        free(new_elem);
                } else if (i == 3) {
                        Pnm_rgb new_elem = make_rgb_pixel(Pb, Pr, Y4);
                        Pnm_rgb ptr = methods->at(closure, col + 1, row + 1);
                        *ptr = *new_elem;
                        free(new_elem);
                }
        }
}

/*
* name:      make_rgb_pixel  
* purpose:   initialize an rgb pixel 
* arguments: the average Pb and Pr values, a float of the Y value
* returns:   the new pixel created off the given information
* effects:   none
*/
Pnm_rgb make_rgb_pixel(float Pb, float Pr, float Y)
{
        /* calculate red, green, and blue values */
        float red = 1.0 * Y + 0.0 * Pb + 1.402 * Pr;
        float green = 1.0 * Y - 0.344136 * Pb - 0.714136 * Pr;
        float blue = 1.0 * Y + 1.772 * Pb + 0.0 * Pr;

        /* make values poitive if negative */
        if (red < 0) {
                red = red - red - red;
        }

        if (green < 0) {
                green = green - green - green;
        } 

        if (blue < 0) {
                blue = blue - blue - blue;
        }

        /* set new element to a new pixel */
        Pnm_rgb new_elem = create_pixel(red, green, blue);

        return new_elem;
}

/*
* name:      create_pixel
* purpose:   assign each member of each pixel
* arguments: floats of the red, green, and blue values
* returns:   a new Pnm_rgb
* effects:   none
*/
Pnm_rgb create_pixel(float red, float green, float blue) 
{
        Pnm_rgb new_elem = malloc(sizeof(struct Pnm_rgb));
        assert(new_elem != NULL);

        /* set red, green, and blue members */
        new_elem->red = red * denom;
        new_elem->green = green * denom;
        new_elem->blue = blue * denom;

        /* if values are larger than the denominator, 
        set to maxval/denominator*/
        if (new_elem->red > denom) {
                new_elem->red = denom;
        }
        if (new_elem->green > denom) {
                new_elem->green = denom;
        } 
        if (new_elem->blue > denom) {
                new_elem->blue = denom;
        }

        return new_elem;
}
