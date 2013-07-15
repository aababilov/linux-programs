#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    setreuid(0, 0);
    setregid(0, 0);
          
    char *bash[] = { "/bin/bash", NULL };
    if (argc > 1)
	execvp(argv[1], argv + 1);
    else 
	execv(bash[0], bash);
    return 0;
}
