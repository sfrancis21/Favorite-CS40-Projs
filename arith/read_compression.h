/* read_compression.h
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This is the header file for the file read_compression.c.
 */


Pnm_ppm read_compressed_image(FILE *input);
UArray2_T get_codeword_array(FILE *input, Pnm_ppm image);
