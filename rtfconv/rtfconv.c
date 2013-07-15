#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char buf[1024 * 32];

void parse_args(int argc, char *argv[])
{
	char *in = NULL, *out = NULL, c;
	while ((c =
		getopt(argc, argv, "i:o:")) != -1) {
		switch (c) {
		case 'i':
			in = optarg;
			break;
		case 'o':
			out = optarg;
			break;			     
		}
	}
	if (optind < argc)
		in = argv[optind];
	if (in)
		freopen(in, "rt", stdin);
	if (out)
		freopen(out, "w+t", stdout);
}

int main(int argc, char *argv[])
{
	int i;
	
	parse_args(argc, argv);
	for (;;) {
		buf[0] = '\0';
		if (fgets(buf, sizeof buf, stdin) == NULL)
		    break;
		if (buf[0] == '\0')
			break;
		for (i = 0; buf[i]; ++i) 
			if (buf[i] == '\\' && buf[i + 1] == '\'') {
				long c = strtol(buf + i + 2, NULL, 16);
				i += 3;
				printf("%c", c & 0xff); 
			} else
				printf("%c", buf[i]);
	}
	return 0;
}
