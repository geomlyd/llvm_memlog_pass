#include <stdlib.h>
#include <stdio.h>

extern void *anotherFunction(int);

void *aFunction(int numBytes){
  return anotherFunction(numBytes);
}

int main(){
  printf("Malloc 100 integers (400 bytes)\n");
  int *x = malloc(sizeof(int)*100);
  void *y = aFunction(900);
  printf("Freeing 900 bytes in main\n");
  free(y);
  printf("Freeing 100 bytes in main\n");
  free(x);
  return 0;
}
