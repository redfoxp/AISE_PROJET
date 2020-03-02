#include "my_malloc.h"

block * Basic_block = NULL;

void* my_malloc(const size_t size) {

void * ptr = NULL;
printf("\nMalloc : Asked for %ld bytes\n\n", size);

if (Basic_block == NULL) {
  if (size <= BLOCK_MIN_SIZE ) {

    Basic_block = mmap(0, BLOCK_MIN_SIZE + sizeof(block), PROT_WRITE | PROT_READ,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (Basic_block == MAP_FAILED)
    {
    printf("mmap allocate error \n");
    return NULL;
    }
    Basic_block->size = BLOCK_MIN_SIZE;
    Basic_block->free_size = BLOCK_MIN_SIZE - size - sizeof(block)- 2* sizeof(chunk);
    Basic_block->next = NULL;
    Basic_block->chunk_nbr = 1;

    printf("debug \n");
    Basic_block->chunk_list = (chunk*)Basic_block + (sizeof(block));
    Basic_block->chunk_list->status = 1;
    ptr = &Basic_block->chunk_list;
    Basic_block->chunk_list->size = size ;
    Basic_block->chunk_list->previous = NULL;

    chunk * new_chunk = Basic_block->chunk_list+ sizeof(chunk)+ size;

    printf("first chunk  : %p \n", &Basic_block->chunk_list);

    Basic_block->chunk_list->next = new_chunk;
    new_chunk->status = 0;
    new_chunk->size = BLOCK_MIN_SIZE - size ;
    new_chunk->previous =  Basic_block->chunk_list;
    new_chunk->next = NULL;

    printf("second chunk : %p \n", Basic_block->chunk_list->next);
    printf("size libre  : %ld\n",Basic_block->chunk_list->next->size );
  }
}
else {
  printf("Not empty \n");
  block * Block_ptr = Basic_block;

  while (Block_ptr != NULL) {
    printf("dans %p  \n", &Block_ptr);
    Block_ptr = Block_ptr->next;
  }

  printf("end else\n");
}
printf("end \n");

return ptr;
}


void gap_search(void * ptr,block * Basic,block * tmp_Basic_block,chunk * tmp_chunk_list,
                size_t size)
{
/*  printf("in gap search\n");
  if (Basic->chunk_list->status == 0 && Basic->chunk_list->size >= size) {
    printf("%ld in block chunk \n", Basic->chunk_list->size);
    chunk * next_chunk = Basic->chunk_list->next;
    chunk * free_chunk;
    ptr = Basic->chunk_list->adr;
    free_chunk->adr = Basic->chunk_list->adr + sizeof(chunk) + size;
    free_chunk->size = Basic->chunk_list->size - size - sizeof(chunk);
    free_chunk->status = 0;
    free_chunk->next = next_chunk;
    free_chunk->previous = Basic->chunk_list;

    Basic->chunk_list->size = size;
    Basic->chunk_list->next = free_chunk;
    Basic->chunk_list->status = 1;

    Basic = tmp_Basic_block;
    Basic ->chunk_list = tmp_chunk_list;
  }
  */
}
