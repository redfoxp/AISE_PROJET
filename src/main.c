#include "my_malloc.h"

#ifndef CONSTANTS
#define ALLOCATED_SIZE 10
#endif

int main(int argc, char const *argv[]) {

  int * i = malloc(ALLOCATED_SIZE);

  return 0;
}
