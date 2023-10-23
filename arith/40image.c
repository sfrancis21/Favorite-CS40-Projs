/* 40image.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This file is the implementation of the 40image program.
 *          It compresses or decompresses the image given by the user.
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
#include "decompression.h"

static void (*compress_or_decompress)(FILE *input) = compress40;


/*
* name:       main
* purpose:    run the command loop and execute the appropriate action depending
*             on the command line arguments
* arguments:  the number of arguments from the command line and an array of  
*             each argument
* returns:    exit_success (0) if program runs successfully 
* effects:    decompresses or compresses depending on the command given
*/
int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        /* set to compression */
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        /* set to decompression */
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }

        /* at most one file on command line */
        assert(argc - i <= 1);

        if (i < argc) {
                /* open file given in command line */
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                /* open file from std in */
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}

/*
* name:      compress40
* purpose:   run compression on the given image file
* arguments: file of image to be compressed
* returns:   none
* effects:   none
*/
void compress40(FILE *input)
{
       /* compress image */
       compression(input);
}

/*
* name:      decompress40   
* purpose:   decompress given file into original image
* arguments: file to be decompressed
* returns:   none
* effects:   none
*/
void decompress40(FILE *input) 
{       
        decom(input);
}



