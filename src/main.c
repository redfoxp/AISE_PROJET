#include "my_malloc.h"

#ifndef CONSTANTS
#define ALLOCATED_SIZE 10
#endif

int main(int argc, char const *argv[]) {

  int * ptr = my_malloc(ALLOCATED_SIZE * sizeof(int));

  printf("\n%p \n", &ptr );

  for (int i = 0; i < ALLOCATED_SIZE; i++) {
    ptr[i] = 23;
    printf(" ptr[%d] : %d \n",i, ptr[i]);
  }

  int * ptr2 = my_malloc(ALLOCATED_SIZE * sizeof(int));
  //int * ptr3 = my_malloc(ALLOCATED_SIZE * sizeof(int));
  //int * ptr4 = my_malloc(ALLOCATED_SIZE * sizeof(int));
  //printf(" %x \n", &ptr2 );

  return 0;
}
