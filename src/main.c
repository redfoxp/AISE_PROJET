#include "my_malloc.h"

#ifndef CONSTANTS
#define ALLOCATED_SIZE 10
#endif

int main(int argc, char const *argv[]) {

  int * i = my_malloc(ALLOCATED_SIZE);

  printf(" %x \n", &i );

  return 0;
}
