#ifndef MY_FREE
#define MY_FREE

#include "my_malloc.h"
#include "structure.h"

// FREE -------------------------------------------------------
void  free(void * ptr);
chunk_t * chunk_search(void * ptr);
void free_chunk(chunk_t * chunk);
void free_chunk_fusion(chunk_t * chunk);
void delete_chunk(chunk_t * chunk);
void delete_block(block_t * block);

#endif
