/* compression.h
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/26/22
 * HW4: arith
 * 
 * Summary: This is the header file for the file compression.c.
 */

typedef struct block *block;
struct  block {
        float chroma_pb;
        float chroma_pr;
        float a;
        signed b;
        signed c;
        signed d;
};

typedef struct video_color *video_color;
struct  video_color {
        float Y;
        float Pb;
        float Pr;
};

typedef struct float_cl *float_cl;
struct float_cl {
        Pnm_ppm image;
        A2Methods_T blocked_methods;
        UArray2b_T new_array;
};


void compression(FILE *input);