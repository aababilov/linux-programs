#include <unistd.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <iconv.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <glib.h>

static char buf[1024 * 32];
char *to = "UTF-8", *from = "UTF8", *nowdir;
int verbose = 0;
iconv_t conv;

char *in_hex(const char *s)
{
	static char buf[1024 * 6],
		digs[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
			    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};	
	int i;
	for (i = 0; s[i] && i < sizeof(buf) / 3; ++i) {
		buf[i * 3] = digs[(s[i] >> 4) & 0xf];
		buf[i * 3 + 1] = digs[s[i] & 0xf];
		buf[i * 3 + 2] = ' ';
	}
	buf[i * 3 - 1] = '\0';
//	printf("%s\t%s\n", s, buf);
	return buf;
}

void
process_it(char *filename)
{
	static struct stat fs;
	int n, i;
	char *virtbuf, *out_p, *in_p;
	static char buf[512 * 32];
	static size_t inbytesleft, outbytesleft;
	static char convbuf[256];

	getcwd(buf, sizeof(buf));
		
	stat(filename, &fs);
	if (S_ISDIR(fs.st_mode)) {
		DIR *curdir;
		struct dirent *dentry;
		GSList *name_list = NULL, *plist;

		curdir = opendir(filename);
		if (!curdir) {
			fprintf(stderr, "cannot open directory %s/%s\n", buf, filename);
			return;
		}
		if (verbose)
			printf("%s/%s\n", buf, filename);
		chdir(filename);
		for (; dentry = readdir(curdir); )
			if (strcmp(dentry->d_name, ".") &&
			    strcmp(dentry->d_name, "..")) {
				name_list = g_slist_append(name_list, strdup(dentry->d_name));
			}
		for (plist = name_list; plist; plist = plist->next) {
			process_it((char*)plist->data);
			free(plist->data);
		}
		chdir("..");
		closedir(curdir);
	} else if (!S_ISREG (fs.st_mode))
		return;
	
	out_p = convbuf;
	in_p = filename;
	inbytesleft = strlen(filename);
	outbytesleft = sizeof(convbuf);
	if (iconv(conv, &in_p, &inbytesleft,
		  &out_p, &outbytesleft) !=
	    (size_t)(-1)) {		
		if (out_p != convbuf)
			*out_p = '\0';
		if (strcmp(filename, convbuf)) {
			if (verbose) {
				printf("\t%s (%s)\n", filename, in_hex(filename));
				printf("\t\t%s (%s)\n", convbuf, in_hex(convbuf));
			}
			rename(filename, convbuf);
		} else
			if (verbose)
				printf("\tignored: %s (%s)\n", filename, in_hex(filename));
	} else {
		fprintf(stderr, "failed to convert %s (%s): %s", filename,
			in_hex(filename), strerror(errno));
		errno = 0;		
	}
}

static void usage(void) {
	printf("Usage: codecfnames [OPTIONS and INPUT-FILES]\n"
	       "Available options:\n"
	       "\t-f, --from-code=encoding\n"
	       "\t-t, --to-code=encoding\n"
	       "\t-v, --verbose\n"
	       "\t-l, --list               \tList known encodings\n"
	       "\t-h, --help\n");	       
}

static void parse_args(int argc, char *argv[])
{
	struct option long_options[] = {
		{"from-code", required_argument, NULL, 'f'},
		{"to-code", required_argument, NULL, 't'},
		{"verbose", no_argument, NULL, 'v'},
		{"list", no_argument, NULL, 'l'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, NULL, 0}
	};
	char c, *in = NULL, *t;
	while ((c =
		getopt_long(argc, argv, "f:t:lvh",
			    long_options, NULL)) != -1) {
		switch (c) {
		case 'f':
			from = optarg;
			break;
		case 't':
			to = optarg;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'l':
			system("iconv -l");
			_exit(0);
		case 'h':
			usage();
			_exit(0);
			break;
		default:
			_exit(1);
		}
	}
}

void process_start(char *start)
{
	int i;
	if (strcmp(start, "/") == 0) {
		fprintf(stderr, "Better not do touch /!\n");
		return;
	}
	if (access(start, F_OK) != 0) {
		fprintf(stderr, "Cannot access %s\n", start);
		return;
	}
	i = strlen(start) - 1;
	if (start[i] == '/')
		start[i--] = '\0';
	if (verbose)
		printf("start: %s\n", start);
	for (; i >= 0 && start[i] != '/'; --i);
	if (i > 0) {
		start[i] = 0;
		chdir(start);
		process_it(start + i + 1);
	} else	
		process_it(start);

}

int main(int argc, char *argv[])
{
	parse_args(argc, argv);
	conv = iconv_open(to, from);
	if (conv == (iconv_t)(-1)) {
		fprintf(stderr, "Cannot convert from %s to %s\n", from, to);
		return 1;
	}

	if (optind < argc) {
		do 
			process_start(argv[optind++]);
		while (optind < argc);
	} else
		process_start(".");
	
	iconv_close(conv);	
	return 0;
}
