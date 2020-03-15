#include <stdio.h>

#include "my_malloc.h"
#include "rdtsc.h"

#ifndef DATA
#define DATA_NMBR 1
#define ALLOCATED_SIZE  100
#define MESURES_NMBR 100
#endif

int main(int argc, char const *argv[]) {

  long mesure_start, mesure_stop, avrg;

  int ** ptr ;

  for (size_t i = 0; i < MESURES_NMBR; i++) {
    ptr = malloc(sizeof(int*) * DATA_NMBR);
    mesure_start = rdtsc();
    for (size_t i = 0; i < DATA_NMBR; i++) {
      ptr[i] = malloc(sizeof(int)*ALLOCATED_SIZE);
    }

    for (size_t i = 0; i < DATA_NMBR; i++) {
      free(ptr[i]);
    }
    mesure_stop= rdtsc();
    free(ptr);
    avrg += mesure_stop-mesure_start;
  }
  avrg = avrg / MESURES_NMBR;

  fprintf(stderr, "mesure %ld \n",avrg);
  return 0;
}
