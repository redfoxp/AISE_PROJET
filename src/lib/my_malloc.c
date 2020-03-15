#include "my_malloc.h"


block_t * Basic_block;
chunk_t * free_list;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

size_t cmptr_malloc;
size_t cmptr_free;
size_t cmptr_realloc;
size_t cmptr_calloc;

void *  malloc(size_t size) {
  pthread_mutex_lock(&mutex);
  if (size < 0) {
    //fprintf(stderr, "Unable to allocate negative or nul quantity \n");
    pthread_mutex_unlock(&mutex);
    return NULL;
  }
  ++cmptr_malloc;
  void * ptr = NULL;
  if (size > BLOCK_MIN_SIZE) {
    ptr = new_block_creation(size);

    pthread_mutex_unlock(&mutex);
    return ptr;
  }
//RESEARCH FOR NEEDED CHUNK -----------------------------------------
    chunk_t * chunk = NULL;
    chunk = research_free_chunk(size);

    if (chunk != NULL) {
      free_list_remove(chunk);
      chunk->status = 1;
      long free_size = chunk->size - size - (2* (sizeof(chunk_t)));
      chunk->size = size;
      if (free_size > 64 ) {
        chunk_t * new_chunk = (void*)chunk + size + sizeof(chunk_t);
        new_chunk->status = 0;
        new_chunk->block = chunk->block;
        new_chunk->size = free_size - sizeof(chunk_t);

        if (chunk->next) {
          chunk->next->previous = new_chunk;
        }
        new_chunk->next = chunk->next;
        new_chunk->previous = chunk;
        chunk->next = new_chunk;
        free_list_add(new_chunk);
        chunk_fusion(new_chunk);
      }
      ptr = (void*)chunk + sizeof(chunk_t);
    }
    else
    {
      ptr = new_block_creation(size);
    }

pthread_mutex_unlock(&mutex);

return ptr;
}

//FUNCTION FOR RESEARCH CHUNK ---------------------------------------

chunk_t * research_free_chunk(size_t size)
{
  chunk_t * tmp_chunk = free_list;

  while (tmp_chunk) {
    if (tmp_chunk->size > size && tmp_chunk->status == 0) {
      return tmp_chunk;
    }
    tmp_chunk = tmp_chunk->next_free;
  }

  return NULL;
}

void free_list_remove(chunk_t *  chunk)
{
  if (!chunk->next_free && !chunk->previous_free) {
      free_list = NULL;
      return;
  }
  if (!chunk->next_free && chunk->previous_free) {
      chunk->previous_free->next_free = NULL;
      return;
  }
  if (chunk->next_free && !chunk->previous_free) {
      free_list = chunk->next_free;
      free_list->previous_free = NULL;
      return;
  }
  if (chunk->next_free && chunk->previous_free) {
      chunk->previous_free->next_free = chunk->next_free;
      chunk->next_free->previous_free = chunk->previous_free;
      return;
  }
}
void free_list_add(chunk_t * chunk)
{
  if (!free_list) {
    free_list = chunk;
    chunk->next_free = NULL;
    chunk->previous_free = NULL;
  }
  else
  {
    chunk_t * tmp = free_list;
      if (chunk->size <= tmp->size) {
        chunk->previous_free = NULL;
        chunk->next_free = tmp;
        tmp->previous_free = chunk;
        free_list = chunk;
      }
    else
    {
      while (tmp) {
        if (tmp->next_free) {
          if (tmp->size <= chunk->size && tmp->next_free->size >= chunk->size) {
            tmp->next_free->previous_free = chunk;
            chunk->previous_free = tmp;
            chunk->next_free = tmp->next_free;
            tmp->next_free = chunk;
            break;
          }
        }
        else if (!tmp->next_free) {
          tmp->next_free = chunk;
          chunk->previous_free = tmp;
          chunk->next_free = NULL;
          break;
        }
        tmp = tmp->next_free;
      }
    }
  }
}

void chunk_fusion(chunk_t * chunk) {
if (chunk) {
  if (chunk->next && chunk->previous) {
    if (chunk->next->status == 0) {
      free_list_remove(chunk);
      chunk->size += chunk->next->size + sizeof(chunk_t);
      free_list_remove(chunk->next);
      chunk_delete(chunk->next);
      free_list_add(chunk);
    }
    if (chunk->previous->status == 0) {
      free_list_remove(chunk);
      chunk->size += chunk->previous->size + sizeof(chunk_t);
      free_list_remove(chunk->previous);
      chunk_delete(chunk->previous);
      free_list_add(chunk);
    }
  }
  else if (chunk->next && !chunk->previous) {
    if (chunk->next->status == 0) {
      free_list_remove(chunk);
      chunk->size += chunk->next->size + sizeof(chunk_t);
      free_list_remove(chunk->next);
      chunk_delete(chunk->next);
      free_list_add(chunk);
    }
  }
  else if (!chunk->next && chunk->previous) {
    if (chunk->previous->status == 0) {
      free_list_remove(chunk);
      chunk->size += chunk->previous->size + sizeof(chunk_t);
      free_list_remove(chunk->previous);
      chunk_delete(chunk->previous);
      free_list_add(chunk);
    }
  }
if (!chunk->next && !chunk->previous) {
    if (chunk->status == 0) {
      free_list_remove(chunk);
        chunk_delete(chunk);
    }
  }
}
}

void chunk_delete(chunk_t * chunk) {
  if (!chunk->next && !chunk->previous) {
    block_delete(chunk->block);
  }
  else if (chunk->next && chunk->previous ) {
    chunk->next->previous = chunk->previous;
    chunk->previous->next = chunk->next;
    chunk->next = NULL;
    chunk->previous = NULL;
  }
  else if (!chunk->next && chunk->previous) {
    chunk->previous->next = NULL;
    chunk->next = NULL;
    chunk->previous = NULL;
  }
  else if (chunk->next && !chunk->previous) {
    chunk->block->chunk_list = chunk->next;
    chunk->block->chunk_list->previous = NULL;
    chunk->next = NULL;
    chunk->previous = NULL;
  }
}
void block_delete(block_t * block)
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
    if (munmap((void*)block,sizeof(block_t)
                            + block->chunk_list->size) == -1 ) {
      //fprintf(stderr, "Block free error\n");
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

  if (size < BLOCK_MIN_SIZE ) {
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
    return NULL;
    }

    new_block->size = allocating_size - sizeof(block_t) - sizeof(chunk_t);
    new_block->next = NULL;
    if (tmp_block_prev) {
      new_block->previous = tmp_block_prev;
      new_block->next = NULL;
      tmp_block_prev->next = new_block;
    }
    else
    {
      Basic_block = new_block;
      new_block->next = NULL;
      new_block->previous = NULL;
    }
    new_block->chunk_list = adr + sizeof(block_t);
    new_block->chunk_list->block = adr;
    new_block->chunk_list->status = 1;
    ptr = adr + sizeof(block_t) + sizeof(chunk_t);
    new_block->chunk_list->size = size;
    new_block->chunk_list->previous = NULL;
    new_block->chunk_list->next = NULL;

    if (allocating_size > size + 2 *sizeof(chunk_t) + sizeof(block_t)) {
      chunk_t * new_chunk = adr + sizeof(block_t) + sizeof(chunk_t)
                            +  sizeof(chunk_t) + size ;
      new_chunk->next = NULL;
      new_chunk->previous = new_block->chunk_list;
      new_block->chunk_list->next = new_chunk;
      new_chunk->size = allocating_size - (2 * sizeof(chunk_t))
                        - sizeof(block_t) - size;
      new_chunk->status = 0;
      new_chunk->block = new_block;
      free_list_add(new_chunk);
    }

  return ptr;
}



// PRINT ------------------------------------------------------
void status_print()
{

  block_t * tmp_block = Basic_block;
if (tmp_block) {

   fprintf(stderr,"\nStructure\n");
   fprintf(stderr,"Block size : %ld \n", sizeof(block_t));
   fprintf(stderr,"Chunk size : %ld \n", sizeof(chunk_t));

    while (tmp_block != NULL) {
       fprintf(stderr,"Block   : %p\n", tmp_block );
      chunk_t *chunk_ptr = tmp_block->chunk_list;
      while(chunk_ptr != NULL)
      {
       fprintf(stderr,"  chunk : %p status : %d size : %ld block %p\n",
                chunk_ptr,
                chunk_ptr->status,
                chunk_ptr->size,
                chunk_ptr->block);
      chunk_ptr = chunk_ptr->next;
      }
      tmp_block = tmp_block->next;
    }
  chunk_t * tmp_free = free_list;
   fprintf(stderr,"\nFREE LIST \n" );
  while (tmp_free) {
     fprintf(stderr,"  chunk : %p status : %d size : %ld block %p next %p previous %p\n",
              tmp_free,
              tmp_free->status,
              tmp_free->size,
              tmp_free->block,
              tmp_free->next_free,
              tmp_free->previous_free);
    tmp_free = tmp_free->next_free;
  }
}
else
fprintf(stderr, "\nData Structure empty\n");

}

void stats()
{
  fprintf(stderr, "\nMallocs  : %ld \nFrees    : %ld\nCallocs  : %ld\nRealloc  : %ld\n",
            cmptr_malloc, cmptr_free, cmptr_calloc, cmptr_realloc );
}


// FREE -------------------------------------------------------

void  free(void * ptr) {

  if (!ptr) {
    //fprintf(stderr, "Unvalid free :(\n");
    return;
  }
  pthread_mutex_lock(&mutex);
  ++cmptr_free;
  chunk_t * chunk;
  chunk = chunk_search(ptr);

  if (chunk != NULL) {
    if (chunk->status == 1) {
      chunk->status = 0;
        free_list_add(chunk);
        chunk_fusion(chunk);
    }
  }
  else
  {
    //fprintf(stderr, "Pointer was not allocated \n");
    pthread_mutex_unlock(&mutex);
    return;
  }

  pthread_mutex_unlock(&mutex);

}

void * chunk_search(void * ptr)
{
  block_t * block = Basic_block;
  chunk_t * chunk_list;
  void * research = ptr - sizeof(chunk_t);

  while (block) {
    chunk_list = block->chunk_list;
    while (chunk_list) {
      if ((void*)chunk_list == research) {
        if (chunk_list->block > 0) {
          return (void*)chunk_list;
        }
        else
        {
          return NULL;
        }
      }
      chunk_list = chunk_list->next;
    }
    block = block->next;
  }
  return NULL;
}


//REALLOC -----------------------------------------------------

void *  realloc(void *ptr, size_t size)
{

  if (!ptr) {
    ptr = malloc(size);
    return ptr;
  }
  ++cmptr_realloc;

  void * new_ptr =   malloc(size);

  pthread_mutex_lock(&mutex);

  chunk_t * ptr_chunk = (void*)ptr - sizeof(chunk_t);
  new_ptr = memcpy(new_ptr, ptr, ptr_chunk->size);

  pthread_mutex_unlock(&mutex);

  free(ptr);

  return new_ptr;
}

//CALLOC ------------------------------------------------------


void *  calloc(size_t nmemb, size_t size)
{
  size_t alloc_size = nmemb * size;

  void * ptr =  malloc(alloc_size);
  pthread_mutex_lock(&mutex);
  ++cmptr_calloc;

  memset(ptr,0, alloc_size);

  pthread_mutex_unlock(&mutex);

  return ptr;
}
