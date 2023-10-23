
/* video_color_array.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the functions to carry out transforming the
 *          image to video color space as part of the compression.
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

void make_floats(int col, int row, A2Methods_UArray2 array2, 
        A2Methods_Object *elem, void *cl);
void rgb_to_color_space(video_color new_elem, float red, 
float green, float blue);

/*
* name:      to_color_space
* purpose:   convert the rgb pixels to component video color space 
* arguments: the Pnm_ppm image to be converted
* returns:   a UArray2 of the pixels containing the video color 
*            space components
* effects:   stores new information in a UArray2 and frees memory associated 
*            with the image passed in
*/
A2Methods_UArray2 to_color_space(Pnm_ppm image) 
{
        /* set element size for new array */
        int size = sizeof(struct video_color);

        /* set methods */
        A2Methods_T blocked_methods = uarray2_methods_blocked;
        int blocksize = 2;
        /* make new array of video_color structs */ 
        UArray2b_T new_array = blocked_methods->new_with_blocksize(
                               image->width, image->height, size, blocksize);

        /* create closure that contains the image, the new array and the 
        methods in order to populate the array */
        float_cl closure = malloc(sizeof(struct float_cl));
        assert(closure != NULL);

        /* set members of closure struct */
        closure->image = image;
        closure->new_array = new_array;
        closure->blocked_methods = blocked_methods;

        /* map UArray2 to populate with component video color space info */
        blocked_methods->map_block_major(new_array, make_floats, closure);
        
        /* free image given */
        Pnm_ppmfree(&image);
        
        /* free the closure */
        free(closure);

        return new_array;
}

/*
* name:      make_floats   
* purpose:   apply function to make each rgb pixel value into a float
* arguments: the current column and row, the UArray2 being mapped, a pointer
*            to the current element, and a void pointer to the closure
* returns:   none
* effects:   populates array with new floating point values
*/
void make_floats(int col, int row, A2Methods_UArray2 array2, 
        A2Methods_Object *elem, void *cl) 
{
        (void)array2;
        float_cl closure = (float_cl)cl;
        Pnm_rgb curr = 
        closure->image->methods->at(closure->image->pixels, col, row);

        /* divide each rgb value by the denominator to get a floating 
        point number */
        float new_red = ((float)curr->red) / 
                        (float)closure->image->denominator;
        float new_green = ((float)curr->green) / 
                        (float)closure->image->denominator;
        float new_blue = ((float)curr->blue) / 
                        (float)closure->image->denominator;

        video_color ptr = elem;

        /* create color space values using the new rgb values */
        rgb_to_color_space(ptr, new_red, new_green, new_blue);
        
}

/*
* name:      rgb_to_color_space
* purpose:   create the component video color space values
* arguments: video_color struct that will store the new values, floats of the
*            red, green, and blue pixel values
* returns:   none
* effects:   sets new members in struct
*/
void rgb_to_color_space(video_color new_elem, float red, float green, 
        float blue) 
{
        /* using the red, green, and blue values to find the unique Y, Pb,
        and Pr values */
        new_elem->Y = 0.299 * red + 0.587 * green + 0.114 * blue; 
        new_elem->Pb = -0.168736 * red - 0.331264 * green + 0.5 * blue; 
        new_elem->Pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;

}