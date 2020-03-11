#include "my_malloc.h"

block_t * Basic_block;
chunk_t * free_list;

void* my_malloc(const size_t size) {
  void * ptr = NULL;
  printf("\nMalloc : Asked for %ld bytes\n\n", size);

//RESEARCH FOR NEEDED CHUNK -----------------------------------------
    chunk_t * new_chunk = NULL;
    new_chunk = research_free_chunk(size);
    if (new_chunk) {
      printf("place found \n");
      ptr = lists_update(new_chunk,size);
    }
    else
    {
      ptr = new_block_creation(size);
    }

//PRINTS ------------------------------------------------------------
status_print();
printf("pointer %zu \n", ptr);

return ptr;
}

//FUNCTION FOR RESEARCH CHUNK ---------------------------------------

chunk_t * research_free_chunk(int size)
{
  chunk_t * tmp_chunk = free_list;

  while (tmp_chunk) {
    if (tmp_chunk->size >= size && tmp_chunk->status == 0) {
      return tmp_chunk;
    }
    tmp_chunk = tmp_chunk->next_free;
  }

  return NULL;
}


//LISTS UPDATE ------------------------------------------------
void * lists_update(chunk_t * chunk, size_t size)
{
  void * ptr = NULL;
  chunk_t * new_chunk = NULL;

    size_t old_size = chunk->size;
    chunk->size = size;
    chunk->status = 1;
    free_list_remove(chunk);
    if (old_size > size + sizeof(chunk_t)) {
      new_chunk = (void*)chunk +  sizeof(chunk_t) + size ;
      new_chunk->next = chunk->next;
      new_chunk->previous = chunk;
      new_chunk->size = old_size - sizeof(chunk_t) - size;
      new_chunk->status = 0;
      new_chunk->block = chunk->block;
      if (chunk->next) {
        chunk->next->previous = new_chunk;
        printf("debug \n");
      }
      chunk->next = new_chunk;
    }
    if (new_chunk) {
      free_list_add(new_chunk);
    }
    ptr = (void*)chunk + sizeof(chunk_t);


  return ptr;
}

void free_list_remove(chunk_t *  chunk)
{
  printf("removing  %zu  \n", chunk);
  chunk_t * tmp_free = free_list;
  while ((void*)tmp_free != (void*)chunk) {
    tmp_free = tmp_free->next_free;
  }
  if (tmp_free->next_free == NULL && tmp_free->previous_free == NULL) {
    free_list = NULL;
  }
  else{
  if (tmp_free->next_free == NULL && tmp_free->previous_free != NULL) {
    tmp_free->previous_free->next_free = NULL;
    chunk->previous_free = NULL;
    chunk->next_free = NULL;
  }
  if (tmp_free->next_free != NULL && tmp_free->previous_free == NULL) {
      free_list = free_list->next_free;
      free_list->previous_free = NULL;
      chunk->previous_free = NULL;
      chunk->next_free = NULL;
  }
  if (tmp_free->next_free != NULL && tmp_free->previous_free != NULL) {
      tmp_free->previous_free->next_free = tmp_free->next_free;
      tmp_free->next_free->previous_free = tmp_free->previous_free;
      chunk->previous_free = NULL;
      chunk->next_free = NULL;
  }
}
}
void free_list_add(chunk_t *  chunk)
{
  chunk_t * tmp_list = free_list;
  if (free_list == NULL) {
    printf("free_list null \n");
    free_list = chunk;
    chunk->next_free = NULL;
    chunk->previous_free = NULL;
  }
  else
  {
    int status_update = 0;
    while (tmp_list && !status_update) {
      if (tmp_list->next_free == NULL && tmp_list->previous_free == NULL) {
        if (tmp_list->size > chunk->size) {
          tmp_list->previous_free = chunk;
          chunk->next_free = tmp_list;
          chunk->previous_free = NULL;
          free_list = chunk;
          status_update = 1;
        }
        else
        {
          tmp_list->next_free = chunk;
          chunk->previous_free = tmp_list;
          chunk->next_free = NULL;
          status_update = 1;
        }
      }
      else if (tmp_list->next_free && tmp_list->previous_free == NULL) {
        if (tmp_list->size >= chunk->size) {
          tmp_list->previous_free = chunk;
          chunk->next_free = tmp_list;
          chunk->previous_free = NULL;
          free_list = chunk;
          status_update = 1;
        }
      }
      else if (tmp_list->next_free == NULL && tmp_list->previous_free) {
        if (tmp_list->size < chunk->size) {
          tmp_list->next_free = chunk;
          chunk->previous_free = tmp_list;
          chunk->next_free = NULL;
          status_update = 1;
        }
        else
        {
          chunk->previous_free = tmp_list->previous_free;
          tmp_list->previous_free->next_free = chunk;
          chunk->next_free = tmp_list;
          tmp_list->previous_free = chunk;
          status_update = 1;
        }
      }
      else if (tmp_list->next_free && tmp_list->previous_free) {
        if (tmp_list->size > chunk->size) {
          chunk->previous_free = tmp_list->previous_free;
          chunk->next_free = tmp_list;
          tmp_list->previous_free->next_free = chunk;
          tmp_list->previous_free = chunk;
          status_update = 1;
        }
      }


    if (!status_update) {
      tmp_list = tmp_list->next_free;
    }
  }
}
}

void * new_block_creation(size_t size)
{
  void * ptr;

  block_t * tmp_block_prev = Basic_block;
  size_t allocating_size;

  if (tmp_block_prev) {
    while (tmp_block_prev->next != NULL) {
      tmp_block_prev = tmp_block_prev->next;
    }
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
    if (tmp_block_prev) {
      new_block->previous = tmp_block_prev;
      tmp_block_prev->next = new_block;
    }
    else
    {
      Basic_block = new_block;
    }
    new_block->chunk_list = adr + sizeof(block_t);
    new_block->chunk_list->block = new_block;
    new_block->chunk_list->status = 1;
    ptr = (void*)new_block->chunk_list + sizeof(chunk_t);
    new_block->chunk_list->size = size;
    new_block->chunk_list->previous = NULL;
    new_block->chunk_list->next = NULL;

    if (allocating_size > size + 2 *sizeof(chunk_t) + sizeof(block_t)) {
      chunk_t * new_chunk = (void*)new_block->chunk_list
                            +  sizeof(chunk_t) + size ;
      new_chunk->next = new_block->chunk_list->next;
      new_chunk->previous = new_block->chunk_list;
      new_block->chunk_list->next = new_chunk;
      new_chunk->size = allocating_size - 2 * sizeof(chunk_t)
                        - sizeof(block_t) - size;
      new_chunk->status = 0;
      new_chunk->block = new_block->chunk_list->block;
      free_list_add(new_chunk);

    }

  return ptr;
}


// FREE -------------------------------------------------------

void my_free(void * ptr) {
  printf("Asked to free %zu \n", ptr);
  chunk_t * chunk = chunk_search(ptr);
  free_chunk(chunk);
}

chunk_t * chunk_search(void * ptr)
{
  block_t * block = Basic_block;
  chunk_t * chunk_list;
  while (block) {
    chunk_list = block->chunk_list;
    while (chunk_list) {
      if ((void*)chunk_list + sizeof(chunk_t) == ptr) {
        printf("free search %zu \n", chunk_list);
        return chunk_list;
      }
      chunk_list = chunk_list->next;
    }
    block = block->next;
  }
  return NULL;
}


void free_chunk(chunk_t * chunk)
{
  if (chunk == NULL) {
    printf("Unvalid free :(\n");
    exit(0);
  }
  chunk->status = 0;
  free_chunk_fusion(chunk);
  printf("chunk next %zu , chunk prev %zu \n", chunk->next, chunk->previous);
  if (chunk->next == NULL && chunk->previous == NULL && chunk->status == 0) {
    printf("block suppretion\n");
    printf("chunk size %ld\n", chunk->size);
    free_list_remove(chunk);
    delete_block(chunk->block);
  }
  status_print();
}
void free_chunk_fusion(chunk_t * chunk)
{
  if (chunk->next) {
    if (chunk->next->status == 0) {
      chunk->size = chunk->size + sizeof(chunk_t) + chunk->next->size;
      free_list_remove(chunk->next);
      delete_chunk(chunk->next);
    }
  }
  if (chunk->previous) {
    if (chunk->previous->status == 0) {
      chunk->size = chunk->size + sizeof(chunk_t) + chunk->previous->size;
      free_list_remove(chunk->previous);
      delete_chunk(chunk->previous);
    }
  }

  free_list_add(chunk);

}

void delete_chunk(chunk_t * chunk)
{
  if (chunk->previous && chunk->next) {
    chunk->previous->next = chunk->next;
    chunk->next->previous = chunk->previous;
    chunk = NULL;
  }
  else if (chunk->previous == NULL && chunk->next) {
    chunk->block->chunk_list = chunk->next;
    chunk->block->chunk_list->previous = NULL;
    chunk = NULL;
  }
  else if (chunk->previous && chunk->next == NULL) {
    chunk->previous->next = NULL;
    chunk = NULL;
  }
  else if (chunk->previous == NULL && chunk->next == NULL) {
    block_t * tmp_block = chunk->block;
    tmp_block->chunk_list = NULL;
  }
}

void delete_block(block_t * block)
{
  int delete_status = 0;
  if (block->previous && block->next && !delete_status) {
    block->next->previous = block->previous;
    block->previous->next = block->next;
    delete_status = 1;
  }
  if (block->previous == NULL && block->next && !delete_status) {
    Basic_block = block->next;
    block->next->previous = NULL;
    delete_status = 1;
  }
  if (block->previous && block->next == NULL && !delete_status) {
    block->previous->next = NULL;
    delete_status = 1;
  }
  if (block->previous == NULL && block->next == NULL && !delete_status) {
    Basic_block = NULL;
    delete_status = 1;
  }
  if (delete_status) {
    if (munmap((void*)block,sizeof(block)
    + block->chunk_list->size) == -1 ) {
      printf("Free error \n");
    }
  }
}







// PRINT ------------------------------------------------------
void status_print()
{
  block_t * tmp_block = Basic_block;

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
  chunk_t * tmp_free = free_list;
  printf("\nFREE LIST \n" );
  while (tmp_free) {
    printf("  chunk : %zu status : %d size : %ld block %zu\n",
              tmp_free,
              tmp_free->status,
              tmp_free->size,
              tmp_free->block);
    tmp_free = tmp_free->next_free;
  }
}
