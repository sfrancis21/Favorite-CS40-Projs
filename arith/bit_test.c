#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "bitpack.h"
#include <stdbool.h>
#include <stdint.h>
#include "except.h"
#include <math.h>

void test_no_width();
void test_big_width();

void test_perfect_width();
void weird_getu();
void big_getu();

void all_zero();
//void get_signed();
void signed_unsigned();

int main() {
        // uint64_t n = 8;
        // bool test = Bitpack_fitsu(n, 4);
        // if (test) {
        //         int answer = 1;
        //         printf("ANSWER: %d\n", answer);
        // } else {
        //         int answer = 0;
        //         printf("ANSWER: %d\n", answer);
        // }

        uint64_t word = 0x3f4;
        //unsigned width = 6;
        unsigned lsb = 2;
        fprintf(stderr, "Word: %ld\n", word);
        int64_t new_word = Bitpack_gets(word, -1, lsb);
        assert(new_word == -3);

        // int64_t new_field = -31;
        // uint64_t final = Bitpack_news(word, width, lsb, new_field);
        // fprintf(stderr, "Word: %ld\n", final);
        // int64_t get_field = Bitpack_gets(final, width, lsb);
        // fprintf(stderr, "Field: %ld\n", get_field);
        
        // assert(get_field == -4);

        // test_no_width();
        // //test_big_width();
        // test_perfect_width();
        // weird_getu();
        // big_getu();
        // all_zero();
        // //get_signed();
        // signed_unsigned();
        //printf("answer: %ld", new_word);
        // 110010
}

void test_no_width() 
{
        uint64_t word = 0x3f4;
        unsigned width = 0;
        unsigned lsb = 2;
        int64_t sub_word = Bitpack_gets(word, width, lsb);
        assert(sub_word == 0);
}

/* make sure correct error gets thrown */
/* void test_big_width() 
{
        uint64_t word = 0x3f4;
        unsigned width = 64;
        unsigned lsb = 2;
        int64_t sub_word = Bitpack_gets(word, width, lsb);
} */

void test_perfect_width() 
{
        uint64_t word = 0x3f4;
        unsigned width = 64;
        unsigned lsb = 0;
        int64_t sub_word = Bitpack_gets(word, width, lsb);
        assert(sub_word == 1012);
}

void weird_getu()
{
        uint64_t word = 0x7B;
        unsigned width = 4;
        unsigned lsb = 0;
        int64_t sub_word = Bitpack_gets(word, width, lsb);
        assert(sub_word == -5);
}

void big_getu()
{
        //uint64_t word = 0x7Bfffff956728179;
        uint64_t word = 0xF7FFFFF2ACE502F2;
        unsigned width = 5;
        unsigned lsb = 59;
        uint64_t sub_word = Bitpack_getu(word, width, lsb);
        fprintf(stderr, "sub_word: %ld\n", sub_word);
        assert(sub_word == 30);
}

void all_zero()
{
        uint64_t word = 0x0000000000000000;
        unsigned width = 5;
        unsigned lsb = 59;
        uint64_t sub_word = Bitpack_getu(word, width, lsb);
        fprintf(stderr, "sub_word: %ld\n", sub_word);
        assert(sub_word == 0);
}

// void get_signed()
// {
//         uint64_t word = 0xF7FFFFF2ACE502F2;
//         unsigned width = 9;
//         unsigned lsb = 23;
//         int64_t sub_word = Bitpack_gets(word, width, lsb);
//         fprintf(stderr, "sub_word: %ld\n", sub_word);
//         assert(sub_word == 345);
// }

void signed_unsigned()
{
        uint64_t word = 0x0;
        unsigned width = 9;
        unsigned lsb = 23;
        word = Bitpack_news(word, width, lsb, 345);
        int64_t sub_word = Bitpack_gets(word, width, lsb);
        assert(sub_word == 345);
}
