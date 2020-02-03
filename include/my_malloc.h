#ifndef  MY_MALLOC_H
#define MY_MALLOC_H

#include <stdio.h>
#include <malloc.h>


void* my_malloc(const size_t size);

#define malloc my_malloc
#endif
