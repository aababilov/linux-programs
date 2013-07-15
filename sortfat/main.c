#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

static char l1;

static int
one (const struct dirent *unused)
{
  return 1;
}

static void
processdir (char *dir)
{
  struct dirent **eps;
  struct stat fs;
  int n, i;
#define MAX_L (1024 * 8)
  char *buf = malloc (MAX_L);
  static char tmp[MAX_L];

  printf("process: %s\n", dir);
  n = scandir (dir, &eps, one, alphasort);
  if (n >= 0) {
    for (i = 0; i < n; ++i) {
      if (strcmp(eps[i]->d_name, ".") == 0 ||
	  strcmp(eps[i]->d_name, "..") == 0)
	continue;
      sprintf (buf, "%s/%s", dir, eps[i]->d_name);
      stat(buf, &fs);
      strcpy(tmp, buf);
      tmp[11] = l1;
      if (S_ISREG(fs.st_mode)) 
	rename(buf, tmp);
      else if (S_ISDIR(fs.st_mode)) {
	mkdir(tmp, 0755);
	processdir(buf);
      }
    }
  }
  else
    fprintf (stderr, "Couldn't open the directory %s\n", dir);
  rmdir(dir);
  free (buf);
}

int main() {
  char c, mediapath[64] = "/media/sda/", tmp[64];
  for (c = 'a'; c <= 'z'; ++c) {
    mediapath[9] = c;
    strcat(strcpy(tmp, mediapath), "music");
    if (access(tmp, F_OK) == 0) {
      strcat(strcpy(tmp, mediapath), "_usic");
      mkdir(tmp, 0755);
      l1 = '_';
      strcat(strcpy(tmp, mediapath), "music");
      processdir(tmp);
      mkdir(tmp, 0755);
      l1 = 'm';
      strcat(strcpy(tmp, mediapath), "_usic");
      processdir(tmp);
      break;
    }
  }
  return 0;

}
