#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <sys/mman.h>
#include <pthread.h>
#include "structure.h"


// MALLOC -----------------------------------------------------
void * malloc(size_t size);
chunk_t * research_free_chunk(int size);
void free_list_remove(chunk_t *  chunk);
void free_list_add(chunk_t * chunk);
void chunk_fusion(chunk_t * chunk);

void * new_block_creation(size_t size);

// FREE -------------------------------------------------------
void  free(void * ptr);
chunk_t * chunk_search(void * ptr);
void chunk_delete(chunk_t * chunk);
void block_delete(block_t * block);


//REALLOC -----------------------------------------------------
void * realloc(void *ptr, size_t size);


//CALLOC ------------------------------------------------------
void * calloc(size_t nmemb, size_t size);
// PRINT ------------------------------------------------------
void status_print();
void stats();
void stats() __attribute__((destructor));
//void status_print() __attribute__((destructor));



#endif
