#include <stdio.h>

#include "my_malloc.h"

#ifndef DATA
#define DATA_NMBR 1000
#define ALLOCATED_SIZE  100
#endif

int main(int argc, char const *argv[]) {

  int ** ptr = malloc(sizeof(int*) * DATA_NMBR);
  for (size_t i = 0; i < DATA_NMBR; i++) {
    ptr[i] = malloc(sizeof(int)*ALLOCATED_SIZE);
  }

  for (size_t i = 0; i < DATA_NMBR; i++) {
    free(ptr[i]);
  }
  free(ptr);
  return 0;
}
