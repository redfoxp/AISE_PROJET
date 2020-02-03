#ifndef  MY_MALLOC_H
#define MY_MALLOC_H

#include <stdio.h>
#include <malloc.h>
#include <sys/mman.h>

#include "structure.h"

void* my_malloc(const size_t size);
void* my_free(void* ptr);
#define malloc my_malloc
#endif
