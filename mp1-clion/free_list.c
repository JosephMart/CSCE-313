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
#include <stdio.h>
#include <stdlib.h>

void FL_remove(FL_HEADER **list_head, FL_HEADER *block) {
    if ((*list_head)->tail == NULL) {
        *list_head = NULL;
    } else if (block->head == NULL) {
        *list_head = block->tail;
        (*list_head)->head = NULL;
    } else if (block->tail == NULL){
        block->head->tail = NULL;
    } else {
        block->head->tail = block->tail;
        block->tail->head = block->head;
    }

    // Clear head and tail in block
    block->head = NULL;
    block->tail = NULL;
}

void FL_add(FL_HEADER **free_list, FL_HEADER *block) {
    // Travers to the end of the free_list
    if (*free_list == NULL) {
        *free_list = block;
        return;
    }

    (*free_list)->head = block;
    block->head = NULL;
    block->tail = *free_list;
    *free_list = block;
}

FL_HEADER* FL_split(FL_HEADER **free_list, FL_HEADER *block, size_t mem_size) {
    if (block->length + sizeof(block) == mem_size)
        return block;
    if (block->length + sizeof(block) < mem_size)
        return NULL;

    FL_HEADER *extra_block = (FL_HEADER *) ((char *) block + mem_size + sizeof(block));
    extra_block->length = block->length - mem_size - sizeof(block);
    block->length = mem_size;
    FL_add(free_list, extra_block);
    return block;
}
