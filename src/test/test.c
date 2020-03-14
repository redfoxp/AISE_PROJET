#include <stdio.h>

#include "my_malloc.h"

#ifndef ALLOCATED_SIZE
#define ALLOCATED_SIZE  200
#endif

int main(int argc, char const *argv[]) {


  int * ptr =   malloc(sizeof(int)* ALLOCATED_SIZE);

  for (int i = 0; i < ALLOCATED_SIZE; i++) {
    ptr[i] = 21;
    //printf("ptr[%d] : %d \n",i,ptr[i]);
  }

  int * ptr2 =   malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr3 =   malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr4 =   malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr5 =   malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr6 =   malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr7 =   malloc(sizeof(int) * ALLOCATED_SIZE);

  free(ptr);
  free(ptr2);
  free(ptr3);
  free(ptr4);
  free(ptr5);
  free(ptr6);
  free(ptr7);

  int * ptr8 =   malloc(sizeof(int) * ALLOCATED_SIZE);
  int * ptr9 =   malloc(sizeof(int) * ALLOCATED_SIZE);


  ptr9 =   realloc(ptr9,sizeof(int) * 2 * ALLOCATED_SIZE);

  for (size_t i = 0; i < 2 * ALLOCATED_SIZE; i++) {
    ptr9[i] = 21;
    //printf("ptr[%d] = %d \n",i, ptr9[i]);
  }

  int * ptr10 = calloc(10,sizeof(int));
  for (size_t i = 0; i < 10; i++) {
    //fprintf(stderr, "ptr10[%d] = %d\n",ptr10[i] );
  }

  free(ptr8);
  free(ptr9);
  free(ptr10);



  //printf("succes :)\n");
  fprintf(stderr, "fin  \n" );
  return 0;
}
