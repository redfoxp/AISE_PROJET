#include "my_malloc.h"


block_t * Basic_block;
chunk_t * free_list;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

size_t cmptr_malloc;
size_t cmptr_free;
size_t cmptr_realloc;
size_t cmptr_calloc;


void *  malloc(const size_t size) {
  pthread_mutex_lock(&mutex);
  if (size <= 0) {
    return NULL;
  }
  ++cmptr_malloc;
  //fprintf(stderr, " entry Malloc  %ld  \n",cmptr_malloc);

  void * ptr = NULL;

   //printf("\nMalloc : Asked for %ld bytes\n\n", size);

//RESEARCH FOR NEEDED CHUNK -----------------------------------------
    chunk_t * new_chunk = NULL;
    new_chunk = research_free_chunk(size);
    if (new_chunk) {
      ptr = lists_update(new_chunk,size);
    }
    else
    {
      ptr = new_block_creation(size);
    }

//PRINTS ------------------------------------------------------------
//status_print();
 //printf("pointer %zu \n", ptr);
 //fprintf(stderr, " Malloc %ld %zu \n",cmptr_malloc,ptr - sizeof(chunk_t));

 //fprintf(stderr, " exit Malloc %ld \n",cmptr_malloc);
pthread_mutex_unlock(&mutex);
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
    if (old_size > size + sizeof(chunk_t)) {
      new_chunk = (void*)chunk +  sizeof(chunk_t) + size ;
      new_chunk->next = chunk->next;
      new_chunk->previous = chunk;
      new_chunk->size = old_size - sizeof(chunk_t) - size;
      new_chunk->status = 0;
      new_chunk->block = chunk->block;
      if (chunk->next) {
        chunk->next->previous = new_chunk;
      }
      chunk->next = new_chunk;
    }
    if (new_chunk) {
      free_list_add(new_chunk);
    }
    ptr = (void*)chunk + sizeof(chunk_t);

    free_list_remove(chunk);

  return ptr;
}

void free_list_remove(chunk_t *  chunk)
{
  chunk_t * tmp_free = free_list;
  while ((void*)tmp_free != (void*)chunk && tmp_free) {
    tmp_free = tmp_free->next_free;
  }
  //fprintf(stderr, "chunk   %zu \n tmp_free %zu \n", chunk,tmp_free);
  if (tmp_free) {
    //fprintf(stderr, "free list free_list_remove\n");

  if (tmp_free->next_free == NULL && tmp_free->previous_free == NULL) {
    free_list = NULL;
  }
  else{
  if (tmp_free->next_free == NULL && tmp_free->previous_free != NULL) {
    tmp_free->previous_free->next_free = NULL;
    chunk->previous_free = NULL;
    chunk->next_free = NULL;
  }
  else if (tmp_free->next_free != NULL && tmp_free->previous_free == NULL) {
      free_list = free_list->next_free;
      free_list->previous_free = NULL;
      chunk->previous_free = NULL;
      chunk->next_free = NULL;
  }
  else if (tmp_free->next_free != NULL && tmp_free->previous_free != NULL) {
      tmp_free->previous_free->next_free = tmp_free->next_free;
      tmp_free->next_free->previous_free = tmp_free->previous_free;
      chunk->previous_free = NULL;
      chunk->next_free = NULL;
  }
}
}
}
void free_list_add(chunk_t *  chunk)
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
        }
        else if (!tmp_list->next_free) {
          tmp_list->next_free = chunk;
          chunk->next_free = NULL;
          chunk->previous_free = tmp_list;
        }
        else if (tmp_list->previous_free) {
          tmp_list->previous_free->next_free = chunk;
          chunk->previous_free = tmp_list->previous_free;
          chunk->next_free = tmp_list;
          tmp_list->previous = chunk;
        }
      }
      //fprintf(stderr, " status_update %d\n",status_update);

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
     fprintf(stderr,"  chunk : %zu status : %d size : %ld block %zu\n",
              tmp_free,
              tmp_free->status,
              tmp_free->size,
              tmp_free->block);
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
    //fprintf(stderr, "Unvalid free \n");
    return;
  }
  pthread_mutex_lock(&mutex);
  ++cmptr_free;
  //fprintf(stderr, " entry Free %ld \n",cmptr_free);
  chunk_t * chunk = chunk_search(ptr);
  //fprintf(stderr, " Free %ld   %zu block %zu \n",cmptr_free, chunk, chunk->block);
  free_chunk(chunk);
  //fprintf(stderr, "after free_chunk \n");
  //status_print();

  //fprintf(stderr, " exit Free %ld  \n",cmptr_free);
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


void free_chunk(chunk_t * chunk)
{
  if (chunk == NULL) {
    fprintf(stderr, " Unvalid free exit \n");
    return;
    //exit(0);
  }
  chunk->status = 0;
  //fprintf(stderr, "before free_chunk_fusion\n");
  free_chunk_fusion(chunk);
  //fprintf(stderr, "afte free_chunk_fusion  \n" );

  if (chunk->next == NULL && chunk->previous == NULL && chunk->status == 0) {
    free_list_remove(chunk);
    delete_block(chunk->block);
  }
}

void free_chunk_fusion(chunk_t * chunk)
{

  if (chunk && chunk->next && chunk->next->status == 0) {
    chunk->size = chunk->size + sizeof(chunk_t) + chunk->next->size;
    free_list_remove(chunk->next);
    delete_chunk(chunk->next);
  }
  if (chunk && chunk->previous && chunk->previous == 0) {
    chunk->size = chunk->size + sizeof(chunk_t) + chunk->previous->size;
    free_list_remove(chunk->previous);
    delete_chunk(chunk->previous);
  }

  if (chunk) {
    free_list_add(chunk);
    //fprintf(stderr, "fusion \n");
  }

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
    chunk = NULL;
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
      fprintf(stderr, "Block free error\n");
    }
  }
}


//REALLOC -----------------------------------------------------

void *  realloc(void *ptr, size_t size)
{

  if (!ptr) {
    ptr = malloc(size);
    return ptr;
  }
  void * new_ptr =   malloc(size);

  pthread_mutex_lock(&mutex);
  ++cmptr_realloc;
  //fprintf(stderr, " entry Realloc  %ld\n", cmptr_realloc);
  //fprintf(stderr, " Realloc %ld  du %zu\n",cmptr_realloc, ptr - sizeof(chunk_t));

  chunk_t * ptr_chunk = (void*)ptr - sizeof(chunk_t);
  new_ptr = memcpy(new_ptr, ptr, ptr_chunk->size);

  pthread_mutex_unlock(&mutex);
  free(ptr);
  //fprintf(stderr, " exit  Realloc  %ld  \n",cmptr_realloc);

  return new_ptr;
}

//CALLOC ------------------------------------------------------


void *  calloc(size_t nmemb, size_t size)
{

  void * ptr =  malloc(nmemb * size);
  pthread_mutex_lock(&mutex);
  ++cmptr_calloc;
  //fprintf(stderr, " entry Calloc %ld \n",cmptr_calloc);

  //fprintf(stderr, " Calloc %ld %zu size %ld \n",cmptr_calloc,ptr, nmemb*size);
  ptr = memset(ptr,0, nmemb*size);


  //fprintf(stderr, " exit Calloc %ld \n",cmptr_calloc);
  pthread_mutex_unlock(&mutex);

  return ptr;
}
