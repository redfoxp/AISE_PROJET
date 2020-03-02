#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <stdio.h>
#include <sys/mman.h>

#include "structure.h"

void* my_malloc(const size_t size);
void* my_free(void* ptr);
void gap_search(void * ptr,block * Basic,block * tmp_Basic_block,chunk * tmp_chunk_list,
                size_t size);

#endif
