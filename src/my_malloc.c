#include "my_malloc.h"

block * Basic_block = NULL;
int BLOCK_NBR = 0;

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
    ++BLOCK_NBR;

    Basic_block->size = BLOCK_MIN_SIZE;
    Basic_block->chunk_list = mmap(0 ,sizeof(chunk),
                                   PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);
    //chunk_list_tmp = Basic_block->chunk_list;
    Basic_block->next = NULL;
    Basic_block->chunk_nbr = 1;

    Basic_block->chunk_list->status = 1;
    ptr = &Basic_block->chunk_list;
    Basic_block->chunk_list->size = size ;
    Basic_block->chunk_list->previous = NULL;

    chunk * new_chunk = mmap(0 ,sizeof(chunk),
                             PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);


    printf("first chunk : %p \n", &Basic_block->chunk_list);
    printf("next before : %p \n", Basic_block->chunk_list->next);

    Basic_block->chunk_list->next = new_chunk;
    new_chunk->status = 0;
    new_chunk->size = BLOCK_MIN_SIZE - size ;
    new_chunk->previous =  Basic_block->chunk_list;
    new_chunk->next = NULL;

    printf("next after  : %p \n", Basic_block->chunk_list->next);
  }
}
else {
  /*
  block * tmp_Basic_block = Basic_block;
  chunk * tmp_chunk_list = Basic_block->chunk_list;

  printf("Block not empty\n");
  printf("DISPONIBLE : \n");
  while(Basic_block->next != NULL){
    while(Basic_block->chunk_list->next != NULL){
       //gap_search(ptr, Basic_block, tmp_Basic_block, tmp_chunk_list, size);
       Basic_block->chunk_list += sizeof(chunk);
    }
    //gap_search(ptr, Basic_block, tmp_Basic_block, tmp_chunk_list, size);

    Basic_block->chunk_list += sizeof(chunk);
    Basic_block += sizeof(block);
  }



  //DERNIER BLOCK
  while(Basic_block->chunk_list->next != NULL){
    //gap_search(ptr, Basic_block, tmp_Basic_block, tmp_chunk_list, size);
    Basic_block->chunk_list += sizeof(chunk);
 }

 if (Basic_block->chunk_list->next == NULL) {
   printf("chunk list null \n");
 }
 printf("debug\n");


  //DERNIER CHUNK DU DERNIER BLOCK
  //gap_search(ptr, Basic_block, tmp_Basic_block, tmp_chunk_list, size);

  //Basic_block = tmp_Basic_block;
  //Basic_block->chunk_list = tmp_chunk_list;
  */
}

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
