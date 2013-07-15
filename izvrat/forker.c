#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main() {
  printf("Hello ");
  fork();
  printf("\n");
  return 0;
}
