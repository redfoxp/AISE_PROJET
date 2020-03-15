#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef CONST
#define BLOCK_MIN_SIZE 4096
#define ADDITIONAL_SIZE 0
#endif

struct chunk_t{
  long allign;
  size_t size;
  struct block_t * block;
  int status;
  struct chunk_t * previous;
  struct chunk_t * next;
  struct chunk_t * next_free;
  struct chunk_t * previous_free;
};
typedef struct chunk_t chunk_t;

struct block_t{
  size_t size;
  chunk_t * chunk_list;
  struct block_t * previous;
  struct block_t * next;
};
typedef struct block_t block_t;


#endif
