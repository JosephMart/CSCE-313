/*
    File: free_list.c

    Author: Joseph Martinsen
            Department of Computer Science
            Texas A&M University
    Date  : <date>

    Modified:

    This file contains the implementation of the module "FREE_LIST".

*/

#include <stddef.h>
#include "free_list.h"

void FL_remove(FL_HEADER *block) {
    // Set tail.head to point to block head
    FL_HEADER *temp = block->tail;
    temp->head = block->head;

    // Set head.tail to point to block tail
    temp = block->head;
    temp->tail = block->tail;

    // Clear head and tail in block
    block->head = NULL;
    block->tail = NULL;
}

void FL_add(FL_HEADER *free_list, FL_HEADER *block) {
    // Travers to the end of the free_list
    FL_HEADER *current = free_list;
    while (current->head != NULL) {
        current = current->head;
    }

    // Add block to the end of free_list
    current->head = block;
}