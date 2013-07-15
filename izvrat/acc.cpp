#include <iostream>
#include <unistd.h>
#include <cstdio>
int main() {
  if (access("delete.me", F_OK)) {
    bool remove=true;
  }
  if (remove) std::cout<<"Please remove 'delete.me'\n";
  return 0;
}
