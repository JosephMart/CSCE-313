/*
    File: free_list.h

    Author: Joseph Martinsen
            Department of Computer Science
            Texas A&M University
    Date  : 08/02/08

    Modified:

*/

#ifndef FREE_LIST_H
#define FREE_LIST_H
typedef struct fl_header { /* header for block in free list */
/* put stuff here */
    unsigned int length;
    struct fl_header *head;
    struct fl_header *tail;
} FL_HEADER;

/* Remove the given block from given free list. The free-list
pointer points to the first block in the free list. Depending
on your implementation you may not need the free-list pointer.*/
void FL_remove(FL_HEADER *block);

/* Add a block to the free list. */
void FL_add(FL_HEADER *free_list, FL_HEADER *block);
#endif //FREE_LIST_H
