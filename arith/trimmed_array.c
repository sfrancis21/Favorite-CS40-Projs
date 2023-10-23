/* trimmed_array.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the functions needed to trim the image as a
 *          step in the compression of the image.
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

A2Methods_UArray2 trimmed_array(Pnm_ppm image, int change_width,
        int change_height);
Pnm_ppm new_pnm(A2Methods_UArray2 new_array, int width, int height, 
        Pnm_ppm image);

/*
* name:      trim   
* purpose:   to trim the width and height of the given image 
* arguments: the Pnm_ppm image to be trimed
* returns:   the trimmed Pnm_ppm 
* effects:   none
*/
Pnm_ppm trim(Pnm_ppm image)
{   
        A2Methods_UArray2 new_array = NULL;
        int width = image->width;
        int height = image->height;

        /* check if image has odd width or height, or both */
        if ((image->width % 2 != 0) && (image->height % 2 != 0)) {
                /* cut off extra column and row */
                new_array = trimmed_array(image, 1, 1);
                width = width - 1;
                height = height - 1;
        } else if (image->width % 2 != 0) {
                /* cut off extra column */
                new_array = trimmed_array(image, 1, 0);
                width = width - 1;
        } else if (image->height % 2 != 0) {
                /* cut off extra row */
                new_array = trimmed_array(image, 0, 1);
                height = height - 1;
        }   

        /* create a new image with the new width and heigth */
        Pnm_ppm new_image = new_pnm(new_array, width, height, image);

        return new_image; 
}

/*
* name:      trimmed_array
* purpose:   create a new trimmed pixel array from the original image
* arguments: the original image to be trimmed, the change in the width and 
*            change in the height
* returns:   a new UArray2 of pixels with the trimmed width and height
* effects:   old elements are now in the same spot in the new array
*/
A2Methods_UArray2 trimmed_array(Pnm_ppm image, int change_width, 
        int change_height) 
{        
        /* set width and height */
        int width = image->methods->width(image->pixels);
        int height = image->methods->height(image->pixels);

        int size = sizeof(struct Pnm_rgb);

        /* loop through old image */
        A2Methods_UArray2 new_array = image->methods->new(
                        image->width - change_width, 
                        image->height - change_height, size);
                for (int r = 0; r < height - change_height; r++) { 
                        for (int c = 0; c < width - change_width; c++) {
                                /* set new element to contents of
                                the old element */
                                Pnm_rgb old_elem = 
                                        image->methods->at(image->pixels,
                                                c, r); 
                                Pnm_rgb new_elem = 
                                        image->methods->at(new_array, 
                                                c, r); 
                                *new_elem = *old_elem;
                        }
                }

        return new_array;
}

/*
* name:      new_pnm
* purpose:   create a new image with the new trimmed array of pixels
* arguments: the UArray2 of pixels, the width & height, and the original image
* returns:   the new Pnm_ppm image 
* effects:   frees the old, original image 
*/
Pnm_ppm new_pnm(A2Methods_UArray2 new_array, int width, int height,
        Pnm_ppm image) 
{
        if (new_array == NULL) {
                /* return original image if new_array is null */
                return image;
        } else {
                /* malloc space for new image */
                Pnm_ppm new_image = malloc(sizeof(struct Pnm_ppm));
                assert(new_image != NULL);

                /* set members of new image */
                new_image->width = width;
                new_image->height = height;
                new_image->denominator = image->denominator;
                new_image->pixels = new_array;
                new_image->methods = image->methods;

                /* free original image */
                Pnm_ppmfree(&image);

                return new_image;
        }
}
