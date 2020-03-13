#include <stdio.h>

#include "my_malloc.h"

#ifndef ALLOCATED_SIZE
#define ALLOCATED_SIZE  2000
#endif

int main(int argc, char const *argv[]) {


  int * ptr =   malloc(sizeof(int)* ALLOCATED_SIZE);

  for (int i = 0; i < ALLOCATED_SIZE; i++) {
    ptr[i] = 21;
    //printf("ptr[%d] : %d \n",i,ptr[i]);
  }

  int * ptr2 =   malloc(sizeof(int) * ALLOCATED_SIZE);


    free(ptr);

  int * ptr3 =   malloc(sizeof(int) * ALLOCATED_SIZE);

  int * ptr4 =   malloc(sizeof(int) * 20);

  int * ptr5=   malloc(sizeof(int) * 20);

  int * ptr6 =   malloc(sizeof(int) * 20);

    free(ptr5);
    free(ptr4);
    free(ptr6);

  int * ptr7 =   malloc(sizeof(int) * 20);
  for (size_t i = 0; i < 20; i++) {
    ptr7[i] = 222;
    }


  ptr7 =   realloc(ptr7,sizeof(int)*30);
  for (size_t i = 0; i < 30; i++) {
    //ptr7[i] = 222;)*
    //printf("ptr : %d  \n",ptr7[i] );
    }

int * ptr8 =   calloc(20,sizeof(int));
for (size_t i = 0; i < 20; i++) {
  //printf("ptr8 : %d \n", ptr8[i]);
}

  //printf("succes :)\n");
  fprintf(stderr, "fin  \n" );
  return 0;
}
