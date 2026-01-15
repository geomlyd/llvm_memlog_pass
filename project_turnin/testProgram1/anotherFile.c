#include <stdio.h>
#include <stdlib.h>

void *anotherFunction(int numBytes){
  printf("Called a function that will malloc %d bytes, then malloc 42 bytes, free those, and return the first %d\n", numBytes, numBytes);
  void *k = malloc(numBytes);
  void *l = malloc(42);
  free(l);
  return k;
}