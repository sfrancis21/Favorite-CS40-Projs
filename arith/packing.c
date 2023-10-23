#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
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

void make_packed_array(int col, int row, A2Methods_UArray2 array2, 
A2Methods_Object *elem, void *cl);

/*
* name:      
* purpose:   
* arguments: 
* returns:    
* effects:  
*/
A2Methods_UArray2 packing_bits(A2Methods_UArray2 uarray, A2Methods_T methods) 
{
        int width = methods->width(uarray);
        int height = methods->height(uarray);
        int size = sizeof(uint64_t);
        UArray2_T new_array = methods->new(width, height, size);

        methods->map_row_major(uarray, make_packed_array, new_array);

        methods->free(&uarray);
        
        return new_array;
}

/*
* name:      
* purpose:   
* arguments: 
* returns:    
* effects:  
*/
void make_packed_array(int col, int row, A2Methods_UArray2 array2, 
A2Methods_Object *elem, void *cl) 
{
        (void)array2;
        
        A2Methods_T methods = uarray2_methods_plain; 

        UArray2_T new_array = (UArray2_T)cl;

        block block_info = (block)elem;
        
        unsigned new_a = round(block_info->a * 63);
        //fprintf(stderr, "OLD C: %d\n", new_a);
        uint64_t word = 0x0;
        word = Bitpack_newu(word, 6, 26, new_a);
        word = Bitpack_news(word, 6, 20, block_info->b);
        //fprintf(stderr, "Orig B: %d\n", block_info->b);
        word = Bitpack_news(word, 6, 14, block_info->c);
        word = Bitpack_news(word, 6, 8, block_info->d);
        word = Bitpack_newu(word, 4, 4, block_info->chroma_pb);
        word = Bitpack_newu(word, 4, 0, block_info->chroma_pr); 
        // int64_t new_b = Bitpack_getu(word, 9, 23);
        // fprintf(stderr, "ORI A: %ld\n", new_b);

        uint64_t *new_elem = methods->at(new_array, col, row);  
        *new_elem = word;
        //fprintf(stderr, "Codeword Compression: %lx\n", word);
        //fprintf("compression word: %ld", word);
}