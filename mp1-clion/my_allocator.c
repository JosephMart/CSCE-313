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
#include "my_allocator.h"
#include "free_list.h"

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

// Stored constant of the header size
#define FL_HEADER_SIZE sizeof(FREE_LIST_HEAD)

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

unsigned int init_allocator(unsigned int _basic_block_size,
                            unsigned int _length) {
    // Initialize memory using malloc
    FREE_LIST_HEAD = (FL_HEADER*) malloc(_length + sizeof(FL_HEADER));

    // Initialize the free list
    if (FREE_LIST_HEAD != NULL && _length >= _basic_block_size) {
        FREE_LIST_HEAD->length = _length;
        FREE_LIST_HEAD->head = NULL;
        FREE_LIST_HEAD->tail = NULL;
        return _length;
    }
    return 0;
}

Addr my_malloc(size_t _length) {
    // Find a block of appropriate size
    FL_HEADER *current = FREE_LIST_HEAD;
    while (current->length < _length + FL_HEADER_SIZE) {
        if (current->tail == NULL) {
            return 0;
        }
        current = current->tail;
    }

    FL_remove(&FREE_LIST_HEAD, current);

    // Split the block as needed
    if (current->length > _length + FL_HEADER_SIZE) {
        FL_split(&FREE_LIST_HEAD, current, _length);
    }
    Addr return_val = (Addr) ((char*) current + FL_HEADER_SIZE);
    return return_val;
}

int my_free(Addr _a) {
    FL_HEADER *block = (FL_HEADER *) ((char *) _a - FL_HEADER_SIZE);
    FL_add(&FREE_LIST_HEAD, block);
    return 0;
}

