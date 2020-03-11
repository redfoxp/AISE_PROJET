#include <stdio.h>
#include "my_malloc.h"

#ifndef ALLOCATED_SIZE
#define ALLOCATED_SIZE  2000
#endif

int main(int argc, char const *argv[]) {


  int * ptr = my_malloc(sizeof(int)* ALLOCATED_SIZE);
  printf("ptr 1   : %zu \n", ptr);
  for (int i = 0; i < ALLOCATED_SIZE; i++) {
    ptr[i] = 21;
    //printf("ptr[%d] : %d \n",i,ptr[i]);
  }

  int * ptr2 = my_malloc(sizeof(int) * ALLOCATED_SIZE);


  my_free(ptr);

  int * ptr3 = my_malloc(sizeof(int) * ALLOCATED_SIZE);

  int * ptr4 = my_malloc(sizeof(int) * 20);

  int * ptr5= my_malloc(sizeof(int) * 20);

  int * ptr6 = my_malloc(sizeof(int) * 20);

  my_free(ptr5);
  my_free(ptr4);
  my_free(ptr6);

  int * ptr7 = my_malloc(sizeof(int) * 20);

  my_free(ptr7);


  printf("succes :)\n");




  return 0;
}
