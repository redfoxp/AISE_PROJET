#include "my_malloc.h"

block * Basic_block = NULL;

void* my_malloc(const size_t size) {
void * ptr = NULL;
//printf("Malloc : \nAsked for %ld bytes\n", size);
if (Basic_block == NULL) {
  printf("Block empty\n\n");
  if (size <= BLOCK_MIN_SIZE ) {

    Basic_block = mmap(NULL, BLOCK_MIN_SIZE + sizeof(block), PROT_WRITE | PROT_READ,
                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (Basic_block == MAP_FAILED)
    {
    printf("mmap allocate error \n");
    return NULL;
    }

    Basic_block[0].index = 0;
    Basic_block[0].adr = &Basic_block;
    Basic_block[0].size = BLOCK_MIN_SIZE;
    Basic_block[0].chunk_list = mmap(NULL ,sizeof(chunk)* 2,
                                     PROT_WRITE | PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);
    Basic_block[0].chunk_list[0].index = 0;
    Basic_block[0].chunk_list[0].status = '1';
    Basic_block[0].chunk_list[0].adr = &ptr;
    Basic_block[0].chunk_list[0].size = size ;
    Basic_block[0].chunk_list[0].next = &Basic_block->chunk_list[1];
    Basic_block[0].chunk_list[0].previous = NULL;

    Basic_block[0].next = NULL;


  }
}
else {
  printf("Block not empty\n");
}

return ptr;
}
