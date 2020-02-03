#ifndef STRUCTURE_H
#define STRUCTURE_H

#ifndef CONST
#define BLOCK_MIN_SIZE 4096;
#endif

typedef struct chunk{
  char status;
  void * adr;
  size_t size;
  chunk * next;
  chunk * previous;
} chunk;

typedef struct block{
  void * adr;
  size_t size;
  chunk * chunk_list;
  block * next;
} block;

#endif
