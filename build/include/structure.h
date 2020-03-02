#ifndef STRUCTURE_H
#define STRUCTURE_H

#ifndef CONST
#define BLOCK_MIN_SIZE 4096
#endif

struct chunk{
  int status;
  size_t size;
  struct chunk * next;
  struct chunk * previous;
};
typedef struct chunk chunk;

struct block{
  size_t size;
  size_t free_size;
  size_t chunk_nbr;
  chunk * chunk_list;
  struct block * next;
};

typedef struct block block;

#endif
