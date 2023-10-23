/* decompression.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file contains the overview of the decompression steps.
 *          It makes the appropriate calls to other files to decompress the 
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
#include "read_compression.h"
#include "pixels_array.h"

void print_pixels(Pnm_ppm image);

/*
* name:      decom 
* purpose:   decompress given image
* arguments: file of image to decompress
* returns:   none
* effects:   calls necessary functions to carry out decompression
*/
void decom(FILE *input)
{
        /* set methods */
        A2Methods_T methods = uarray2_methods_plain; 

        /* call to function in read_compression.c to make new Pnm_ppm image */
        Pnm_ppm image = read_compressed_image(input);

        /* call to function in read_compression.c to make array of codewords */
        UArray2_T codewords = get_codeword_array(input, image);

        /* call to function in pixels_array.c to create final image */
        Pnm_ppm final_image = float_to_rgb(image, codewords, methods);

        /* print final image */
        print_pixels(final_image);
}

/*
* name:      print_pixels
* purpose:   print out final image 
* arguments: image to be printed
* returns:   none
* effects:   none
*/
void print_pixels(Pnm_ppm image) 
{
        /* write to stdout */
        Pnm_ppmwrite(stdout, image);

        /* free image */
        Pnm_ppmfree(&image);
}