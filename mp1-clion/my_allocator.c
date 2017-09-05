/* 
    File: my_allocator.c

    Author: <your name>
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
static char *start;
static unsigned int remaining;

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include<stdlib.h>
#include "my_allocator.h"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FUNCTIONS FOR MODULE MY_ALLOCATOR */
/*--------------------------------------------------------------------------*/

/* Don't forget to implement "init_allocator" and "release_allocator"! */
unsigned int init_allocator(unsigned int _basic_block_size,
                            unsigned int _length) {
    start = (char*) malloc(_length);
    remaining = _length;

    return (start) ? _length : 0;
}


Addr my_malloc(size_t _length) {
    /* This preliminary implementation simply hands the call over the
       the C standard librarinity!
       Of course this needs to be replaced by your implementation.
    */
    if (_length <= remaining) {
        Addr old_start = (Addr)(start);
        start = start + _length;
        remaining = remaining - (unsigned int)(_length);
        return old_start;
    }
    return NULL;
}

int my_free(Addr _a) {
    /* Same here! */
    free(_a);
    return 0;
}

