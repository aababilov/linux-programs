#include <stdio.h>
int main() {
  int x[10];
  int i;
  i=1;
  i[x] = 10;
  *(i+x)=100;
  printf("%d", x[1]);
  return 0;
}
