#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <unistd.h>

#include "ArgsParse.hpp"

using namespace std;

namespace {
	void usage(void) {
		printf("Usage: planarity [OPTIONS] [INPUT-FILE]\n"
		       "Available options:\n"
		       "\t-i, --input=file      \tFile for input\n"
		       "\t-e, --embed=file      \tOutput file for embedded on plane graph\n"
		       "\t-t, --triangulate=file\tOutput file for triangulated graph\n"
		       "\t-c, --coordinates=file\tOutput file for coordinates of vertices\n"
		       "\t-V, --version         \tPrint version\n"
		       "\t-h, --help            \tShow this message\n"
		       "If no input file is specified, reading from stdin.\n"
		       "If no output file is specified in long option, writing to stdout.\n"
		       "If graph is not planar 1 is returned\n");
	}
}

namespace ArgsParse {
	char *in, *embed = NOT_NEED, *trian = NOT_NEED, *coords = NOT_NEED;
}

void ArgsParse::parse_args(int argc, char *argv[])
{
	struct option long_options[] = {
		{"input", required_argument, NULL, 'i'},
		{"embed", optional_argument, NULL, 'e'},
		{"triangulate", optional_argument, NULL, 't'},
		{"coordinates", optional_argument, NULL, 'c'},
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, NULL, 0}
	};
	char c;
	while ((c =
		getopt_long(argc, argv, "i:e:t:c:vh",
			    long_options, NULL)) != -1) {
		switch (c) {
		case 'i':
			in = optarg;
			break;
		case 'e':
			embed = optarg;
			break;
		case 't':
			trian = optarg;
			break;
		case 'c':
			coords = optarg;
			break;
		case 'V':
			printf("1.0\n");
			_exit(0);
		case 'h':
			usage();
			_exit(0);
			break;
		default:
			_exit(1);
		}
	}
	if (optind < argc)
		in = argv[optind];
}

