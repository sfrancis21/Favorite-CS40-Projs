/* 
 * uarray2b.c
 * Juliana Rosen (jrosen33) and Sara Francis (sfranc07), 10/3/22
 * HW3: locality
 * 
 * Summary: This file contains implementation of the uarray2b functions as 
 * defined in the uarray2b.h interface.  It closely follows our uarray2
 * interface, with the added complexity that can handle the blocking of 
 * 2D array. 
 */

#include "uarray2.h"
#include <uarray2b.h>
#include <stdlib.h>
#include "uarray.h"
#include <stdio.h>
#include <assert.h>

struct UArray2b_T {
        int width; /* number of columns */
        int height; /* length of columns (number of rows) */ 
        int size; /* number of bytes in one element */
        int blocksize; /* square root of the number of elements in a block */
        UArray2_T array2d; 
};


/*
* name:       UArray2b_addUArray
* purpose:    apply function that ads a new UArray at the given location in the
*             UArray2, creating the blocks
* arguments:  the current column and row, the uarray2 being mapped, a void
*             pointer to the current element, and a void pointer to the
*             closure
* returns:    none - void 
* effects:    a new UArray will be at each index of the UArray2 passed in 
*/
void UArray2b_addUArray(int col, int row, UArray2_T uarray2, 
void *elem, void *cl) 
{
        (void)col;
        (void)row;
        (void)uarray2;

        assert(uarray2 != NULL);
        assert(elem != NULL);
        assert(cl != NULL);

        UArray2b_T array2b = (UArray2b_T)cl;

        int length = array2b->blocksize * array2b->blocksize;
        
        UArray_T new_uarray = UArray_new(length, array2b->size);

        UArray_T *array = elem;

        *array = new_uarray;
}


/*
* name:       UArray2b_new
* purpose:    initialize a new blocked 2d array
* arguments:  the desired width and height, the size of the elements to be
*             stored, and the blocksize (sqroot of the number of cells in a
*             block).
* returns:    a new UArray2b 
* effects:    blocksize < 1 is a checked runtime error and a new UArray2b 
*             is created 
*/
extern UArray2b_T UArray2b_new (int width, int height, int size, 
int blocksize) 
{
        UArray2b_T new_array = malloc(sizeof(struct UArray2b_T));
        assert(new_array != NULL);

        new_array->width =  width;
        new_array->height = height;
        new_array->size = size;
        new_array->blocksize = blocksize;

        int array2_width = width;
        int array2_height = height;

        if (array2_width % blocksize != 0) {
                array2_width = (array2_width / blocksize) + 1;
        } else {
                array2_width = (array2_width / blocksize);
        }
        
        if (array2_height % blocksize != 0) {
                array2_height = (array2_height / blocksize) + 1;
        } else {
                array2_height = (array2_height / blocksize);
        }

        new_array->array2d = UArray2_new(array2_width, array2_height, 
        sizeof(UArray_T));
        UArray2_map_row_major(new_array->array2d, UArray2b_addUArray, 
        new_array);

      
        return new_array;

}


/*
* name:       UArray2b_new_64K_block
* purpose:    initialize a new blocked 2d array where a block occupies at most
*             64KB
* arguments:  the desired width and height, the size of the elements to be
*             stored
* returns:    a new UArray2b with blocks occupying at most 64KB 
* effects:    new UArray2b is created  
*/
extern UArray2b_T UArray2b_new_64K_block(int width, int height, int size)
{
        /* 65,526 bytes whose squareroot is 256 */
        int blocksize = 256 / size;

        /* if single cell will not fit in one 64K block, then blocksize is 1 */
        if (blocksize == 0) {
                blocksize = 1;
        }

        UArray2b_T new_array = UArray2b_new (width, height, size, blocksize);
        return new_array;
}

/*
* name:       free_uarray
* purpose:    an apply function to free the given UArray2 and its elements
* arguments:  the current column and row, the UArray2 to be freed, a void
*             pointer to the current element, a void pointer to the 
*             closure
* returns:    none 
* effects:    the memory associated with the UArray2 is freed   
*/
void free_uarray(int col, int row, UArray2_T uarray2, void *elem, void *cl)
{
        (void)col;
        (void)row;
        (void)cl;
        (void)uarray2;
        UArray_T uarray = *(UArray_T *)elem;
        UArray_free(&uarray);
}


/*
* name:       UArray2b_free
* purpose:    an apply function to free the given UArray2b and its elements
* arguments:  the UArray2b to be freede
* returns:    none 
* effects:    the memory associated with the UArray2b is freed   
*/
extern void UArray2b_free (UArray2b_T *array2b)
{
        assert(*array2b != NULL);
        UArray2_map_row_major((*array2b)->array2d, free_uarray, NULL),
        UArray2_free(&(*array2b)->array2d);
        free(*array2b);
}

/*
* name:       UArray2b_width
* purpose:    returns the width of the UArray2b
* arguments:  the UArray2b to be examined
* returns:    the width of the UArray2b as an integer
* effects:    none   
*/
extern int UArray2b_width (UArray2b_T array2b) 
{
        assert(array2b != NULL);
        return array2b->width;
}

/*
* name:       UArray2b_height
* purpose:    returns the height of the UArray2b
* arguments:  the UArray2b to be examined
* returns:    the height of the UArray2b as an integer
* effects:    none   
*/
extern int UArray2b_height (UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->height;
}

/*
* name:       UArray2b_size
* purpose:    returns the size of the elements within the UArray2b
* arguments:  the UArray2b to be examined
* returns:    the size of the elements of the UArray2b as an integer
* effects:    none   
*/
extern int UArray2b_size (UArray2b_T array2b) 
{
        assert(array2b != NULL);
        return array2b->size;
}

/*
* name:       UArray2b_blocksize
* purpose:    returns the blocksize of the UArray2b
* arguments:  the UArray2b to be examined
* returns:    the blocksize of the UArray2b as an integer
* effects:    none   
*/
extern int UArray2b_blocksize(UArray2b_T array2b)
{
        assert(array2b != NULL);
        return array2b->blocksize;
}

/*
* name:       UArray2b_at
* purpose:    returns the element at the given column and row of the given
*             UArray2b
* arguments:  the UArray2b to be examined, the column and row
* returns:    a void pointer to the retrieved element
* effects:    index is out of range is a checked runtime error   
*/
extern void *UArray2b_at(UArray2b_T array2b, int column, int row)
{
        assert(array2b != NULL);
        int width = array2b->width;
        int height = array2b->height;
        
        if ((column > width) || (column < 0)) {   
                return NULL;
        } else if ((row > height) || (row < 0)) {
                return NULL;
        }
        int uarray2_col = (column / array2b->blocksize);

        int uarray2_row = (row / array2b->blocksize);

        UArray_T *block = UArray2_at(array2b->array2d, uarray2_col, 
        uarray2_row);

        int uarrayindex = array2b->blocksize * (row % array2b->blocksize)
                         + (column % array2b->blocksize);

        void *elem = UArray_at(*block, uarrayindex);

        return elem;
}


/*
* name:       UArray2b_map
* purpose:    map through the given UArray2b by visiting every cell in one 
*             block before moving on to the next block
* arguments:  the UArray2b to be examined, the apply function, and the void
*             pointer to the closure
* returns:    none
* effects:    apply function is applied to each element of the UArray2b   
*/
extern void UArray2b_map(UArray2b_T array2b, 
     void apply(int col, int row, UArray2b_T array2b, void *elem, void *cl), 
     void *cl)
{
        assert(array2b != NULL);

        /* length of UArray in block */
        int length = array2b->blocksize * array2b->blocksize;
        int blocksizeBig = array2b->blocksize;
        for (int blockRow = 0; blockRow < UArray2_height(array2b->array2d); 
        blockRow++) {

                for (int blockCol = 0; blockCol < 
                UArray2_width(array2b->array2d); blockCol++) {

                        void *elem = UArray2_at(array2b->array2d, blockCol, 
                        blockRow);
                        for (int index = 0; index < length; index++)  {
                                UArray_T array = *(UArray_T *)elem;
                                /* have to figure out how to access the 
                                col and row */
                                int col_index = (blocksizeBig * blockCol) + 
                                (index % blocksizeBig);
                                //printf("col_index: %d\n", col_index);
                                int row_index = (blocksizeBig * blockRow) + 
                                (index / blocksizeBig);
                                //printf("row_index: %d\n", row_index);

                                if (col_index < array2b->width && row_index < 
                                array2b->height) {
                                        void *num = UArray_at(array, index);
                                        apply(col_index, row_index, array2b, 
                                        num, cl);
                                }

                        }
                 }
         }
}



