/*
    File: my_allocator.c

    Author: Joseph Martinsen
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified: 

    This file contains the implementation of the module "MY_ALLOCATOR".

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include<stdlib.h>
#include <math.h>
#include "my_allocator.h"
#include "free_list.h"
#include "utils.h"

// TODO: remove below
#include<stdio.h>


/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/
// Points to head of the linked list
static FL_HEADER *FREE_LIST_HEAD;
static FL_HEADER **FREE_LIST_ARRAY;

// Stored constant of the header size
#define FL_HEADER_SIZE sizeof(FREE_LIST_HEAD)

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */
// log2(length) - log2(blocksize) + 1
/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

unsigned int init_allocator(unsigned int _basic_block_size,
                            unsigned int _length) {
    // Create Array of FL_HEADERS
    int n = (int) determine_array_size(_basic_block_size, _length);
    FL_HEADER* temp[n];
    FREE_LIST_ARRAY = temp;
    long binary_of_length = convert_to_binary(_length);

    // Create blocks accordinaly
    int power_2_bbs = (int) _log2(_basic_block_size);
    long binary_check = binary_of_length;

    // Get rid of undeeded binary bits for constructing
    int iter = 0;
    while (iter < power_2_bbs) {
        binary_check /= 10;
        iter++;
    }

    printf("\nbinary_check: %ld\n\n", sizeof(FL_HEADER));
    int digit_count = (int) get_digit_count(binary_check);

    for (int i = 0; i < digit_count; i++) {
//        printf("MEMORY ADDRESS: %p\n", (void*) &FREE_LIST_ARRAY[i]);
//        FL_HEADER *current_block = FREE_LIST_ARRAY[i];

        // Create the header
        size_t _len = (size_t) pow(2, power_2_bbs + i);
        FL_HEADER *current_block = malloc(sizeof(FL_HEADER));
        current_block->length = _len;
        current_block->head = NULL;
        current_block->tail = NULL;

        if (binary_check % 10) {
            // create a block
            printf("Create Block: %d\n", (int) _len);
            FL_HEADER *additional_block = malloc(_len + sizeof(FL_HEADER));
            additional_block->length = _len;
            additional_block->head = current_block;
            additional_block->tail = NULL;
            current_block->tail = additional_block;
        } else {
            // Create only a header
            printf("Index: %d\n", (int) pow(2, power_2_bbs + i));
        }
        FREE_LIST_ARRAY[i] = current_block;
        binary_check /= 10;
    }

    // test printing
    for(int i = 0; i < 12; i++){
        printf("header_test_len: %zu\n", FREE_LIST_ARRAY[i]->length);
    }
    return 0;
}

Addr my_malloc(size_t _length) {
    // Find a block of appropriate size

    for(int i = 0; i < 12; i++){
//        int test2 = i;
        size_t test = FREE_LIST_ARRAY[i]->length;
        printf("header_test_len: %zu\n", test);
    }

//    for(int i = 0; i < 3; i++){
//        printf("\ni:%i\n",i);
//        int lol = (int) i;
//        printf("header_test_len: %zu\n", FREE_LIST_ARRAY[lol]->length);
//    }
//    int index = get_index(_length, FREE_LIST_ARRAY);
//    fprintf("\n\nindex: %d", index);
    return (Addr) FREE_LIST_ARRAY[0];

//    FL_HEADER *current = FREE_LIST_HEAD;
//    while (current->length < _length + FL_HEADER_SIZE) {
//        if (current->tail == NULL) {
//            return 0;
//        }
//        current = current->tail;
//    }
//
//    FL_remove(&FREE_LIST_HEAD, current);
//
//    // Split the block as needed
//    if (current->length > _length + FL_HEADER_SIZE) {
//        FL_split(&FREE_LIST_HEAD, current, _length);
//    }
//    Addr return_val = (Addr) ((char*) current + FL_HEADER_SIZE);
//    return return_val;
}

int my_free(Addr _a) {
    FL_HEADER *block = (FL_HEADER *) ((char *) _a - FL_HEADER_SIZE);
    FL_add(&FREE_LIST_HEAD, block);
    return 0;
}

