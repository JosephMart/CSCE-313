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

    printf("\nbinary_check: %ld\n\n", binary_check);
    int digit_count = (int) get_digit_count(binary_check);

    for (int i = 0; i < digit_count; i++) {
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

    // WHY DOES THIS WORK
    long long int sum = 0;
    for(int i = 0; i < 12; i++){
        printf("header_test_len: %zu", FREE_LIST_ARRAY[i]->length);
        if(FREE_LIST_ARRAY[i]->tail) {
            sum += FREE_LIST_ARRAY[i]->length;
            printf("\t EXTRA BLOCK CREATED");
        }
        printf("\n");
    }
    printf("Sum of Blocks Created: %llu\n\n", sum);
    return 0;
}

Addr my_malloc(size_t _length) {
    // Find a block of appropriate size

    // WHY DOES THIS NOT WORK - i gets optimized out............. :(
    long long int sum = 0;
    for(int i = 0; i < 12; i++){
        FL_HEADER *temp = FREE_LIST_ARRAY[i];
        printf("header_test_len: %zu", temp->length);
        if(FREE_LIST_ARRAY[i]->tail) {
            sum += FREE_LIST_ARRAY[i]->length;
            printf("\t EXTRA BLOCK CREATED");
        }
        printf("\n");
    }
    printf("Sum of Blocks Created: %llu\n\n", sum);

    int index = get_index(_length, FREE_LIST_ARRAY);
    FL_HEADER *free_list_row = FREE_LIST_ARRAY[index];
    if (free_list_row->length > _length) {
        // if row needs to be split
        return (Addr) FL_split(&free_list_row, free_list_row->tail, _length) - sizeof(FL_HEADER);
    } else {
        // Size is just what the doctor ordered
        return (Addr) FL_remove(&free_list_row, free_list_row->tail) - sizeof(FL_HEADER);
    }
}

int my_free(Addr _a) {
    FL_HEADER *block = (FL_HEADER *) ((char *) _a + FL_HEADER_SIZE);
    FL_add(&FREE_LIST_ARRAY, block);
    return 0;
}

