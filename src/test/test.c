#include <stdio.h>
#include "my_malloc.h"

#ifndef ALLOCATED_SIZE
#define ALLOCATED_SIZE  100000
#endif

int main(int argc, char const *argv[]) {


  int * ptr = my_malloc(sizeof(int)* ALLOCATED_SIZE);
  printf("ptr 1   : %zu \n", ptr);
  for (int i = 0; i < ALLOCATED_SIZE; i++) {
    ptr[i] = 21;
    //printf("ptr[%d] : %d \n",i,ptr[i]);
  }
  printf("succes \n");

  int * ptr2 = my_malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr3 = my_malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr4 = my_malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr5 = my_malloc(sizeof(int) * ALLOCATED_SIZE);
  //printf("ptr 2   : %zu \n", ptr2);

  return 0;
}
