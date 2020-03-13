#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#include <sys/mman.h>
#include <pthread.h>
#include "structure.h"


// MALLOC -----------------------------------------------------
void * malloc(const size_t size);

void * first_alloc(int size);
chunk_t * research_free_chunk(int size);

void * lists_update(chunk_t * chunk, size_t size);
void free_list_remove(chunk_t *  chunk);
void free_list_add(chunk_t *  chunk);
void * new_block_creation(size_t size);

// FREE -------------------------------------------------------
void  free(void * ptr);
chunk_t * chunk_search(void * ptr);
void free_chunk(chunk_t * chunk);
void free_chunk_fusion(chunk_t * chunk);
void delete_chunk(chunk_t * chunk);
void delete_block(block_t * block);


//REALLOC -----------------------------------------------------
void * realloc(void *ptr, size_t size);


//CALLOC ------------------------------------------------------
void * calloc(size_t nmemb, size_t size);
// PRINT ------------------------------------------------------
void status_print();
void stats();
void stats() __attribute__((destructor));



#endif
