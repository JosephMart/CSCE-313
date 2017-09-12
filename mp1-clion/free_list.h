//
// Created by joseph on 9/12/17.
//

#ifndef FREE_LIST_H
#define FREE_LIST_H
typedef struct fl_header { /* header for block in free list */
/* put stuff here */
} FL_HEADER;

/* Remove the given block from given free list. The free-list
pointer points to the first block in the free list. Depending
on your implementation you may not need the free-list pointer.*/
void FL_remove(FL_HEADER *free_list, FL_HEADER *block);

/* Add a block to the free list. */
void FL_add(FL_HEADER *free_list, FL_HEADER *block);
#endif //FREE_LIST_H
