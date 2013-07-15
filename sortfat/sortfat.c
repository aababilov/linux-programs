#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

static int one (const struct dirent *unused)
{
	return 1;
}

static void processdir (char *dir)
{
	struct dirent **eps;
	struct stat fs;
	int n, i;
#define MAX_L (1024 * 8)
	
	static char tmp[MAX_L];
	printf("process: %s\n", dir);
	chmod(dir, 0777);
	n = scandir(dir, &eps, one, alphasort);
	if (n < 0) {
		perror("scandir");
		return;
	}
	char *buf = malloc(MAX_L);	
	for (i = 0; i < n; ++i) {
		if (strcmp(eps[i]->d_name, ".") == 0 ||
		    strcmp(eps[i]->d_name, "..") == 0)
			continue;
		if (snprintf(buf, MAX_L, "%s/%s", dir, eps[i]->d_name) >= MAX_L) {
			fprintf(stderr, "not enough memory for file name");
			continue;
		}
		stat(buf, &fs);
		strcpy(tmp, buf);
		tmp[11] = 'm';
		if (S_ISREG(fs.st_mode)) 
			rename(buf, tmp);
		else if (S_ISDIR(fs.st_mode)) {
			errno = 0;
			mkdir(tmp, 0777);
			if (errno == EEXIST)
				chmod(tmp, 0777);
			else if (errno) {
				perror(tmp);
				continue;
			} 
			processdir(buf);
		}
	}
	free(buf);
	rmdir(dir);
}

int main(int argc, char *argv[])
{
	char c, musicpath[64] = "/media/sda/music", tmp[64];
	for (c = 'a'; c <= 'z'; ++c) {
		musicpath[9] = c;
		if (access(musicpath, F_OK) == 0) {
			strcpy(tmp, "/media/sda/_usic");
			tmp[9] = c;
			rename(musicpath, tmp);
			mkdir(musicpath, 0777);
			processdir(tmp);			
			return 0;
		}
	}
	fprintf(stderr, "no /media/sd?/music found\n");
	return 0;

}
