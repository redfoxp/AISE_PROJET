#include "my_malloc.h"

block_t * Basic_block;
free_chunk_t * free_list;

void* my_malloc(const size_t size) {

  void * ptr = NULL;
  printf("\nMalloc : Asked for %ld bytes\n\n", size);

  if (Basic_block == NULL) {
//IF FIRST ALLOCATION -----------------------------------------------
    ptr = first_alloc(size);
    printf("first_alloc \n");
  }
  else {
//RESEARCH FOR NEEDED CHUNK -----------------------------------------

    chunk_t * new_chunk = NULL;
    new_chunk = research_chunk(size);
    if (new_chunk) {
      ptr = lists_update(new_chunk,size);
    }
    else
    {
      ptr = new_block_creation(size);
    }
  }

//PRINTS ------------------------------------------------------------
status_print();
printf("pointer %zu \n", ptr);

return ptr;
}

//FUNCTION FOR FIRST ALLOCATION -------------------------------------

void * first_alloc(int size)
{
  void * ptr = NULL;
  size_t allocating_size = 0;
  printf("First allocation\n");
  if (size <= BLOCK_MIN_SIZE ) {
    allocating_size = BLOCK_MIN_SIZE;
  }

  else
  {
    allocating_size = size + ADDITIONAL_SIZE ;
  }

//ALLOCATION OF BLOCK_LIST --------------------------------------

  void * adr = mmap(0, allocating_size + sizeof(block_t)+sizeof(chunk_t),
                    PROT_WRITE | PROT_READ,
                    MAP_PRIVATE | MAP_ANONYMOUS,
                    -1, 0);


  Basic_block = adr;

  if (adr == MAP_FAILED)
  {
  printf("mmap allocate error \n");
  return NULL;
  }

  Basic_block->size = allocating_size + sizeof(block_t) + sizeof(chunk_t);
  Basic_block->next = NULL;
  Basic_block->previous = NULL;
  Basic_block->chunk_list = adr + sizeof(block_t);
  Basic_block->chunk_list->block = Basic_block;
  Basic_block->chunk_list->status = 1;
  ptr = (void*)Basic_block->chunk_list + sizeof(chunk_t);
  Basic_block->chunk_list->size = size ;
  Basic_block->chunk_list->free_list_ptr = NULL;
  Basic_block->chunk_list->previous = NULL;

//CREATION OF FREE CHUNK ----------------------------------------

  if (allocating_size == BLOCK_MIN_SIZE &&
      allocating_size - size  > sizeof(block_t) + sizeof(chunk_t) ) {

  chunk_t * new_chunk = adr + (sizeof(block_t)) + sizeof(chunk_t)+ size;

  new_chunk->status = 0;
  new_chunk->block = Basic_block;
  new_chunk->size = allocating_size - size - sizeof(block_t)
                    - sizeof(chunk_t) ;
  new_chunk->previous =  Basic_block->chunk_list;
  new_chunk->next = NULL;

  Basic_block->chunk_list->next = new_chunk;
  Basic_block->chunk_list->next->previous = Basic_block->chunk_list;



//CREATION OF FREE CHUNK LIST CHUNK -----------------------------

  void * free_list_adr = mmap(0, sizeof(free_chunk_t),
                              PROT_WRITE | PROT_READ,
                              MAP_PRIVATE | MAP_ANONYMOUS,
                              -1, 0);
  free_list = free_list_adr;
  free_list->chunk = Basic_block->chunk_list->next;
  new_chunk->free_list_ptr = free_list;
  free_list->previous = NULL;
  free_list->next = NULL;
}
else
{
  Basic_block->chunk_list->next = NULL;
}
  return ptr;
}

//FUNCTION FOR RESEARCH CHUNK ---------------------------------------

chunk_t * research_chunk(int size)
{
  printf("\nResearch  :\n");
  free_chunk_t * free_list_tmp = free_list;

  while (free_list_tmp != NULL) {
    if (free_list_tmp->chunk->size >= size) {
      printf("good\n");
      return free_list_tmp->chunk;
    }
      free_list_tmp = free_list_tmp->next;
  }

  if (free_list_tmp != NULL) {
    printf("Succes research\n");
    return free_list_tmp->chunk;
  }
  else
  {
    printf("Failed research\n");
    return NULL;

  }
}

//FUNCTION INSERTION FREE_CHUNK -------------------------------



//LISTS UPDATE ------------------------------------------------
void * lists_update(chunk_t * chunk, size_t size)
{
  void * ptr = NULL;
  if (chunk != NULL) {
    size_t old_size = chunk->size;
    chunk->status = 1;
    chunk->size = size;
    chunk_t * new_chunk = (void*)chunk + size + sizeof(chunk_t);
    new_chunk->next = chunk->next;
    new_chunk->previous = chunk;
    new_chunk->size = old_size - sizeof(chunk_t)- size;
    new_chunk->status = 0;
    new_chunk->block = chunk->block;
    if (chunk->next) {
      chunk->next->next->previous = new_chunk;
    }
    chunk->next = new_chunk;
    ptr = (void*)chunk + sizeof(chunk_t);
    free_list_update(chunk,new_chunk);

  }
  else
  {
    printf("need new block\n");

  }
  return ptr;
}

void free_list_update(chunk_t * old_chunk, chunk_t * new_chunk)
{
  free_chunk_t * tmp_list;
  if (old_chunk != NULL) {
    tmp_list = old_chunk->free_list_ptr;

    if (tmp_list->next == NULL && tmp_list->previous == NULL) {
      free_list = NULL;
      munmap((void*)free_list,sizeof(free_chunk_t));
    }
    else
    {
      if (tmp_list->previous) {
        if (tmp_list->next) {
          tmp_list->previous->next = tmp_list->next;
          tmp_list->next->previous = tmp_list->previous;
        }
        else
        {
          tmp_list->previous->next = NULL;
        }
      }
      if (tmp_list->next) {
        if (tmp_list->previous) {
          tmp_list->next->previous = tmp_list->previous;
          tmp_list->previous->next = tmp_list->next;
        }
        else
        {
          tmp_list->next->previous = NULL;
        }
      }
    }
    munmap((void*)tmp_list,sizeof(free_chunk_t));
  }


  if (free_list == NULL) {
    void * free_list_adr = mmap(0, sizeof(free_chunk_t),
                                PROT_WRITE | PROT_READ,
                                MAP_PRIVATE | MAP_ANONYMOUS,
                                -1, 0);
    free_list = free_list_adr;
    free_list->chunk = new_chunk;
    new_chunk->free_list_ptr = free_list;
    free_list->previous = NULL;
    free_list->next = NULL;
  }
  else
  {
    tmp_list = free_list;
    while (tmp_list->chunk->size <= new_chunk->size && tmp_list->next != NULL) {
    tmp_list = tmp_list->next;
    }
    if (tmp_list->chunk->size <= new_chunk->size) {
      void * adr = mmap(0, sizeof(free_chunk_t),
                                  PROT_WRITE | PROT_READ,
                                  MAP_PRIVATE | MAP_ANONYMOUS,
                                  -1, 0);

      free_chunk_t * new_elem = adr;
      new_elem->chunk = new_chunk;
      new_chunk->free_list_ptr = new_elem;
      new_elem->previous = tmp_list;
      new_elem->next = tmp_list->next;
      tmp_list->next = new_elem;
    }
  }


}
void * new_block_creation(size_t size)
{
  void * ptr;

  block_t * tmp_block_prev = Basic_block;
  size_t allocating_size;
  while (tmp_block_prev->next != NULL) {
    tmp_block_prev = tmp_block_prev->next;
  }

  if (size <= BLOCK_MIN_SIZE ) {
    allocating_size = BLOCK_MIN_SIZE;
  }

  else
  {
    allocating_size = size + ADDITIONAL_SIZE ;
  }

  //ALLOCATION OF BLOCK --------------------------------------

    void * adr = mmap(0, allocating_size + sizeof(block_t)+sizeof(chunk_t),
                      PROT_WRITE | PROT_READ,
                      MAP_PRIVATE | MAP_ANONYMOUS,
                      -1, 0);


    block_t * new_block = adr;


    if (adr == MAP_FAILED)
    {
    printf("mmap allocate error \n");
    return NULL;
    }

    new_block->size = allocating_size + sizeof(block_t) + sizeof(chunk_t);
    new_block->next = NULL;
    new_block->previous = tmp_block_prev;
    tmp_block_prev->next = new_block;
    new_block->chunk_list = adr + sizeof(block_t);
    new_block->chunk_list->block = new_block;
    new_block->chunk_list->status = 1;
    ptr = (void*)new_block->chunk_list + sizeof(chunk_t);
    new_block->chunk_list->size = size ;
    new_block->chunk_list->free_list_ptr = NULL;
    new_block->chunk_list->previous = NULL;

    if (allocating_size == BLOCK_MIN_SIZE &&
        allocating_size - size  > sizeof(block_t) + sizeof(chunk_t) ) {

    chunk_t * new_chunk = adr + (sizeof(block_t)) + sizeof(chunk_t) + size;

    new_chunk->status = 0;
    new_chunk->block = new_block;
    new_chunk->size = allocating_size - size - sizeof(block_t)
                      - sizeof(chunk_t) ;
    new_chunk->previous =  new_block->chunk_list;
    new_chunk->next = NULL;

    new_block->chunk_list->next = new_chunk;
    new_block->chunk_list->next->previous = new_block->chunk_list;


    free_list_update(NULL,new_chunk);
  }
  else
  {
    new_block->chunk_list->next = NULL;
  }


  return ptr;
}



void status_print()
{
  block_t * tmp_block = Basic_block;
  free_chunk_t * free_list_tmp = free_list;

  printf("\nStructure\n");
  printf("Block size : %ld \n", sizeof(block_t));
  printf("Chunk size : %ld \n", sizeof(chunk_t));

    while (tmp_block != NULL) {
      printf("Block   : %zu\n", tmp_block );
      chunk_t *chunk_ptr = tmp_block->chunk_list;
      while(chunk_ptr != NULL)
      {
      printf("  chunk : %zu status : %d size : %ld block %zu\n",
                chunk_ptr,
                chunk_ptr->status,
                chunk_ptr->size,
                chunk_ptr->block);
      chunk_ptr = chunk_ptr->next;
      }
      tmp_block = tmp_block->next;
    }

    while (free_list_tmp != NULL)
    {
    printf("free list : block : %zu  chunk %zu size %ld \n",
            free_list_tmp->chunk->block,
            free_list_tmp->chunk,
            free_list_tmp->chunk->size);
            free_list_tmp = free_list_tmp->next;
    }

}
