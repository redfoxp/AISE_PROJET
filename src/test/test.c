#include <stdio.h>
#include "my_malloc.h"

#ifndef ALLOCATED_SIZE
#define ALLOCATED_SIZE  10
#endif

int main(int argc, char const *argv[]) {


  int * ptr = my_malloc(ALLOCATED_SIZE);
  for (int i = 0; i < ALLOCATED_SIZE; i++) {
    ptr[i] = 23;
    printf("ptr[%d] : %d \n",i,ptr[i]);
  }

  int * ptr2 = my_malloc(ALLOCATED_SIZE);
  //int * ptr3 = my_malloc(ALLOCATED_SIZE);
/*
  for (int i = 0; i < ALLOCATED_SIZE; i++) {
    ptr2[i] = 22;
    printf("ptr[%d] : %d \n",i,ptr2[i]);
  }
*/

  return 0;
}
