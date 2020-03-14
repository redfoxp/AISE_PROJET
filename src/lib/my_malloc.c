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

  if (size <= 0) {
    fprintf(stderr, "Unable to allocate negative or nul quantity \n");
    return NULL;
  }
  ++cmptr_malloc;
  //fprintf(stderr, "entry malloc %ld   \n",cmptr_malloc);

  void * ptr = NULL;
  if (size > BLOCK_MIN_SIZE) {
    ptr = new_block_creation(size);

    pthread_mutex_unlock(&mutex);
    return ptr;
  }
//RESEARCH FOR NEEDED CHUNK -----------------------------------------
    chunk_t * chunk = NULL;
    //status_print();
    chunk = research_free_chunk(size);
    //fprintf(stderr, "debug\n");
    if (chunk != NULL) {
      free_list_remove(chunk);
      chunk->status = 1;
      long free_size = chunk->size - size - (2* (sizeof(chunk_t)));
      chunk->size = size;
      if (free_size > 0 ) {
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

//fprintf(stderr, "exit malloc %ld \n",cmptr_malloc);

pthread_mutex_unlock(&mutex);
//status_print();
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
  chunk_t * tmp_list = free_list;
  if (free_list == NULL) {
    free_list = chunk;
    chunk->next_free = NULL;
    chunk->previous_free = NULL;
  }
  else
  {
    int status_update = 0;

    while (tmp_list && !status_update) {
      if (tmp_list->size > chunk->size) {
        if (!tmp_list->previous_free) {
          chunk->previous_free = NULL;
          chunk->next_free = tmp_list;
          tmp_list->previous_free = chunk;
          free_list = tmp_list;
          status_update = 1;
        }
        else if (!tmp_list->next_free) {
          tmp_list->next_free = chunk;
          chunk->next_free = NULL;
          chunk->previous_free = tmp_list;
          status_update = 1;
        }
        else if (tmp_list->previous_free) {
          tmp_list->previous_free->next_free = chunk;
          chunk->previous_free = tmp_list->previous_free;
          chunk->next_free = tmp_list;
          tmp_list->previous = chunk;
          status_update = 1;
        }
      }

    if (!status_update) {
      tmp_list = tmp_list->next_free;
    }

  }
}
}

void chunk_fusion(chunk_t * chunk) {
  if (chunk) {
  if (chunk->next && chunk->next->size > 0) {
    if (chunk->next->status == 0) {
      free_list_remove(chunk);
      chunk->size = chunk->size + chunk->next->size + sizeof(chunk_t);
      //fprintf(stderr, "in fusion 1 %zu  taille %ld  \n",chunk->next,chunk->next->size);
      //fprintf(stderr, "in fusion 2\n");
      free_list_add(chunk);
      free_list_remove(chunk->next);
      chunk_delete(chunk->next);
    }
  }
  if (chunk->previous && chunk->previous->size > 0) {
    if (chunk->previous->status == 0) {
      free_list_remove(chunk);
      chunk->size = chunk->size + chunk->previous->size + sizeof(chunk_t);
      free_list_add(chunk);
      free_list_remove(chunk->previous);
      chunk_delete(chunk->previous);
    }
  }
  else if (!chunk->next && !chunk->previous) {
      free_list_remove(chunk);
      block_delete(chunk->block);
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
  }
  else if (!chunk->next && chunk->previous) {
    chunk->previous->next = NULL;
  }
  else if (chunk->next && !chunk->previous) {
    chunk->next->previous = NULL;
    chunk->block->chunk_list = chunk->next;
  }
}
void block_delete(block_t * block) {
  if (!block) {
    fprintf(stderr, "Block free error \n");
    return;
  }
  else
  {
    if (block->next && block->previous) {
      block->previous->next = block->next;
      block->next->previous = block->previous;
    }
    else if (!block->next && block->previous) {
      block->previous->next = NULL;
    }
    else if (block->next && !block->previous) {
      Basic_block = block->next;
      Basic_block->previous = NULL;
    }
    else if (!block->next && !block->previous) {
      Basic_block = NULL;
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
      new_chunk->next = NULL;
      new_chunk->previous = new_block->chunk_list;
      new_block->chunk_list->next = new_chunk;
      new_chunk->size = allocating_size - 2 * sizeof(chunk_t)
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

   fprintf(stderr,"\nStructure\n");
   fprintf(stderr,"Block size : %ld \n", sizeof(block_t));
   fprintf(stderr,"Chunk size : %ld \n", sizeof(chunk_t));

    while (tmp_block != NULL) {
       fprintf(stderr,"Block   : %zu\n", tmp_block );
      chunk_t *chunk_ptr = tmp_block->chunk_list;
      while(chunk_ptr != NULL)
      {
       fprintf(stderr,"  chunk : %zu status : %d size : %ld block %zu\n",
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
     fprintf(stderr,"  chunk : %zu status : %d size : %ld block %zu next %zu previous %zu\n",
              tmp_free,
              tmp_free->status,
              tmp_free->size,
              tmp_free->block,
              tmp_free->next_free,
              tmp_free->previous_free);
    tmp_free = tmp_free->next_free;
  }
  fprintf(stderr, "Mallocs  : %ld \nFrees    : %ld\nCallocs  : %ld\nRealloc  : %ld\n",
            cmptr_malloc, cmptr_free, cmptr_calloc, cmptr_realloc );
}

void stats()
{
  fprintf(stderr, "Mallocs  : %ld \nFrees    : %ld\nCallocs  : %ld\nRealloc  : %ld\n",
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
  //fprintf(stderr, "entry free %ld \n",cmptr_free);
  chunk_t * chunk;
  chunk = chunk_search(ptr);

  if (chunk != NULL) {
    if (chunk->status == 0) {
      free_list_add(chunk);
      chunk_fusion(chunk);
      pthread_mutex_unlock(&mutex);
      return;
    }
  }
  else
  {
    fprintf(stderr, "Pointer was not allocated \n");
    return;
  }

  //status_print();
  //fprintf(stderr, "exit free %ld \n",cmptr_free);
  pthread_mutex_unlock(&mutex);

}

chunk_t * chunk_search(void * ptr)
{
  block_t * block = Basic_block;
  chunk_t * chunk_list;
  while (block) {
    chunk_list = block->chunk_list;
    while (chunk_list) {
      if ((void*)chunk_list + sizeof(chunk_t) == ptr) {
        return chunk_list;
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
  //fprintf(stderr, "entry realloc %ld  \n",cmptr_realloc);

  void * new_ptr =   malloc(size);

  pthread_mutex_lock(&mutex);

  chunk_t * ptr_chunk = (void*)ptr - sizeof(chunk_t);
  new_ptr = memcpy(new_ptr, ptr, ptr_chunk->size);

  pthread_mutex_unlock(&mutex);

  free(ptr);
  //fprintf(stderr, "exit realloc %ld\n ",cmptr_realloc);

  return new_ptr;
}

//CALLOC ------------------------------------------------------


void *  calloc(size_t nmemb, size_t size)
{
  size_t alloc_size = nmemb * size;

  void * ptr =  malloc(alloc_size);
  pthread_mutex_lock(&mutex);
  ++cmptr_calloc;
  //fprintf(stderr, "entry calloc %ld size %ld \n",cmptr_calloc,alloc_size);

  memset(ptr,0, alloc_size);

  pthread_mutex_unlock(&mutex);
  void * chunk_adr = ptr - sizeof(chunk_t);
  chunk_t * chunk = chunk_adr;
  //fprintf(stderr, "taille chunk %ld \n",chunk->size );

  //fprintf(stderr, "exit calloc %ld %zu \n",cmptr_calloc,ptr);
  return ptr;
}
