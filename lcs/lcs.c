#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/mman.h>

void die(int errcode, const char *fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
	exit(errcode);
}

void *xmalloc (size_t size)
{
	void *r = malloc(size);
	if (!r)
		die(1, "cannot allocate memory: %s\n", strerror(errno));
	return r;
}
	
typedef struct {
	char *name;
	char *mem;
	int fd;
} in_file_t;

in_file_t f1, f2;
int lcs_file, dist = -1;
char *lcs_name = NULL;

#define FILE1 1
#define FILE2 2
#define SIZE1 3
#define SIZE2 4

void usage()
{
	printf("usage: lcs file_a file_b [ OPTIONS ]\n"
	       "\t-o file3\n"
	       "\t--size1 l\n"
	       "\t--size2 l\n"
	       "\t-s|--size l\n"
	       "\t-d| --distance l\n");
	exit(0);
}

void parse_opts(int argc, char *argv[])
{
	struct option long_options[] = {
		{"file1", required_argument, NULL, FILE1},
		{"file2", required_argument, NULL, FILE2},
		{"size2", required_argument, NULL, SIZE1},
		{"size1", required_argument, NULL, SIZE2},
		{"size", required_argument, NULL, 's'},
		{"distance", required_argument, NULL, 'd'},
		{"lcs-file", required_argument, NULL, 'o'},
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, NULL, 0}
	};
	char c;
	while ((c =
		getopt_long(argc, argv, "o:d:s:Vh",
			    long_options, NULL)) != -1) {
		switch (c) {
		case FILE1:
			f1.name = optarg;
			break;
		case FILE2:
			f2.name = optarg;
			break;
		case 'd':
			dist = atoi(optarg);
			break;
		case 'o':
			lcs_name = optarg;
			break;
		case 'V':
			printf("0.1\n");
			exit(0);
		case 'h':
			usage();
		}
	}
	if (optind < argc)
		f1.name = argv[optind++];
	if (optind < argc)
		f2.name = argv[optind++];
	if (!f1.name || !f2.name)
		usage();
}

void open_in_file(in_file_t *f)
{
	struct stat sb;
	if (strcmp(f->name, "-") == 0) {
		f->fd = STDIN_FILENO;
	} else {
		f->fd = open(f->name, O_RDONLY);
		if (f->fd == -1) 
			die(1, "cannot open `%s': %s\n", f->name, strerror(errno));
	}
	if (fstat(f->fd, &sb) == -1) {      
		die(1, "cannot fstat `%s': %s\n", f->name, strerror(errno));
	}	
}

void open_files()
{
	open_in_file(&f1);
	open_in_file(&f2);
}

void find_lcs()
{
	int **l, *dta;	
	int i, j, lcs_len, m, n;
	dist = 20;
	dta = (int *)xmalloc(sizeof(int) * dist * dist);
	l = (int **)xmalloc(sizeof(int*) * dist);
	f1.mem = (char *)xmalloc(sizeof(char) * dist);
	f2.mem = (char *)xmalloc(sizeof(char) * dist);

	m = read(f1.fd, f1.mem, sizeof(char) * dist);
	n = read(f2.fd, f2.mem, sizeof(char) * dist);
	
	for (i = 0; i < m; ++i)
		l[i] = &dta[i * n];
	
	for (i = 0, j = dist + 1; i < m && j < n; ++i, ++j)
		l[i][j] = 0;
	for (i = dist + 1, j = 0; i < m && j < n; ++i, ++j)
		l[i][j] = 0;
	
	for (i = 0; i < m; ++i)
		l[i][0] = f1.mem[i] == f2.mem[0];
	for (j = 0; j < n; ++j)
		l[0][j] = f1.mem[0] == f2.mem[j];
	for (i = 1; i < m; ++i) {
		int last = n - i - 1 < dist ? n : i + 1 + dist;
		for (j = i - dist < 1 ? 1 : i - dist; j < last; ++j) {
			if (f1.mem[i] == f2.mem[j]) {
				l[i][j] = l[i - 1][j - 1] + 1;
			} else {
				l[i][j] = l[i][j - 1] > l[i - 1][j] ?
					l[i][j - 1] : l[i - 1][j];
			}
			printf("%d %d: %d\n", i, j, l[i][j]);
		}
	}
	for (i = 0; i < m; ++i, printf("\n"), (i % 4 == 0 ? printf("-\n"): 0))
		for (j = 0; j < n; ++j, (j % 4 == 0 ? printf("|"): 0))
			printf("%4d", l[i][j]);
	
	/*
	lcs_len = l[f1.len - 1][f2.len - 1];
	if (strcmp(lcs_name, "-")) {
		printf("%d\n", lcs_len);
		lcs_file = open(lcs_name, O_CREAT|O_NOCTTY|O_TRUNC, 0644);
		if (lcs_file == -1)
			die(1, "cannot open `%s': %s\n", lcs_name, strerror(errno));		
	} else {
		lcs_file = STDOUT_FILENO;
	}
	if (lcs_len) {
		int out_len = lcs_len;
		char *out = (char*)l[f1.len - 1];	
		i = f1.len - 1;
		j = f2.len - 1;
		do {
			if (f1.mem[i] == f2.mem[j]) {
				out[--out_len] = f1.mem[i];
				--j;
				--i;
			} else if (l[i][j - 1] > l[i - 1][j])
				--j;
			else
				--i;
		} while (out_len);
		write(lcs_file, out, lcs_len);
	}
	*/
	close(lcs_file);
	free(dta);
	free(l);
}

void find_lcs_length()	
{
	
}

int main(int argc, char *argv[])
{
	parse_opts(argc, argv);
	open_files();
	if (lcs_name)
		find_lcs();
	else
		find_lcs_length();
	close(f1.fd);
	close(f2.fd);
	return 0;
}
