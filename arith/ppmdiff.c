/* ppmdiff.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/14/22
 * HW3: locality
 * 
 * Summary: 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "assert.h"
#include "pnm.h"
#include "uarray2.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "math.h"


int main(int argc, char *argv[])
{
        A2Methods_T methods = uarray2_methods_plain; 

        FILE *fp;
        FILE *newfp;
        assert(argc <= 3);
        if (argc == 3) {
                if (strcmp(argv[1], "-") == 0) {
                        fp = stdin;  
                } else {
                        fp = fopen(argv[1], "r");
                        assert(fp != NULL); 
                }
                if (strcmp(argv[2], "-") == 0) {
                        newfp = stdin;
                } else {
                        newfp = fopen(argv[2], "r");
                        assert(newfp != NULL);
                }
        } 

        Pnm_ppm image = Pnm_ppmread(fp, methods); 
        Pnm_ppm newimage = Pnm_ppmread(newfp, methods);
        
        int image_width = image->width;
        int image_height = image->height;
        int newimage_width = newimage->width;
        int newimage_height = newimage->height;

        if (abs(image_width - newimage_width) > 1) {
                fprintf(stderr, "Width diff > 1\n");
                printf("1.0\n");
        } else if (abs(image_height - newimage_height) > 1) {
                fprintf(stderr, "Height diff > 1\n");
                printf("1.0\n");
        }

        int width;
        int height;
        
        if (image_width > newimage_width) {
                width = newimage_width;
        } else {
                width = image_width;
        }

        if (image_height > newimage_height) {
                height = newimage_height;
        } else {
                height = image_height;
        }    

        double sum = 0;
        double redpixSum, bluepixSum, greenpixSum;
        double redpix, bluepix, greenpix;
        double newredpix, newbluepix, newgreenpix;

        for (int col = 0;  col < width; col++) {
                for (int row = 0; row < height; row++) {
                        Pnm_rgb image_rgb = UArray2_at(image->pixels, col, row); 
                        Pnm_rgb newimage_rgb = UArray2_at(newimage->pixels, col, row); 
                        
                        redpix = (double)(image_rgb->red) / image->denominator;
                        bluepix = (double)(image_rgb->blue) / image->denominator;
                        greenpix = (double)(image_rgb->green) / image->denominator;
                        newredpix = (double)(newimage_rgb->red) / newimage->denominator;
                        newbluepix = (double)(newimage_rgb->blue) / newimage->denominator;
                        newgreenpix = (double)(newimage_rgb->green) / newimage->denominator;
                        
                        redpixSum = (redpix - newredpix) * (redpix - newredpix);
                        bluepixSum = (bluepix - newbluepix) * (bluepix - newbluepix);
                        greenpixSum = (greenpix - newgreenpix) * (greenpix - newgreenpix);
                        
                        sum += redpixSum + bluepixSum + greenpixSum;
        
                        

                }
        }
        
        sum = sum / (3 * width * height);
        sum = sqrt(sum);
        printf("%f\n", sum);    
}
