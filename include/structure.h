#ifndef STRUCTURE_H
#define STRUCTURE_H

#ifndef CONST
#define BLOCK_MIN_SIZE 4096
#define ADDITIONAL_SIZE 160
#endif

struct chunk_t{
  int status;
  struct block_t * block;
  struct free_chunk_t * free_list_ptr;
  size_t size;
  struct chunk_t * previous;
  struct chunk_t * next;
};
typedef struct chunk_t chunk_t;

struct block_t{
  size_t size;
  chunk_t * chunk_list;
  struct block_t * previous;
  struct block_t * next;
};
typedef struct block_t block_t;

struct free_chunk_t {
  chunk_t * chunk;
  struct free_chunk_t * previous;
  struct free_chunk_t * next;
};
typedef struct free_chunk_t free_chunk_t;

#endif
