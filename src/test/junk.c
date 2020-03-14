
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
    free_list_remove(chunk);
    if (new_chunk) {
      free_list_add(new_chunk);
    }
    ptr = (void*)chunk + sizeof(chunk_t);


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
      fprintf(stderr, " status_update %d\n",status_update);

    if (!status_update) {
      tmp_list = tmp_list->next_free;
    }

  }
}
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

void free_list_add(chunk_t * chunk)
{
  chunk_t * tmp = free_list;
  int update_status = 0;
  if (!tmp) {
    free_list = chunk;
    chunk->next_free = NULL;
    chunk->previous_free = NULL;
    update_status = 1;
  }
  else
  {
    while (tmp && !update_status) {
      if (tmp && !tmp->next_free && !tmp->previous_free && !update_status) {
        if (tmp->size >= chunk->size) {
          chunk->previous_free = NULL;
          chunk->next_free = tmp;
          free_list = chunk;
          update_status = 1;
        }
        else
        {
          chunk->previous_free = tmp;
          tmp->next_free = chunk;
          chunk->next_free = NULL;
          update_status = 1;
        }
      }
      else if (tmp && tmp->next_free && !tmp->previous_free && !update_status) {
        if (tmp->size <= chunk->size && tmp->next_free->size >= chunk->size) {
          chunk->previous_free = tmp;
          chunk->next_free = tmp->next_free;
          tmp->next_free->previous_free =chunk;
          tmp->next_free = chunk;
          update_status = 1;
        }
        else {
          chunk->next_free = tmp;
          chunk->previous_free = NULL;
          tmp->previous_free = chunk;
          free_list = chunk;
          update_status = 1;
        }
      }
      else if (tmp && !tmp->next_free && tmp->previous_free && !update_status) {
        if (tmp->size >= chunk->size && tmp->previous_free->size <= chunk->size ) {
          chunk->previous_free = tmp->previous_free;
          chunk->next_free = tmp;
          tmp->previous_free->next_free = chunk;
          tmp->previous_free = chunk;
          update_status = 1;
        }
        else
        {
          tmp->next_free = chunk;
          chunk->previous_free = tmp;
          chunk->next_free = NULL;
          update_status = 1;
        }
      }
      tmp = tmp->next_free;
    }
  }
  if (!update_status) {
    fprintf(stderr, "Update_error %zu \n",chunk);
  }
  //status_print();
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
