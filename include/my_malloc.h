#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdio.h>
#include <sys/mman.h>

#include "structure.h"

void * my_malloc(const size_t size);
void * my_free(void* ptr);

void * first_alloc(int size);
chunk_t * research_chunk(int size);

void * lists_update(chunk_t * chunk, size_t size);
void free_list_update(chunk_t * old_chunk, chunk_t * new_chunk);
void * new_block_creation(size_t size);


void status_print();



#endif
