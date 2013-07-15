#ifndef ARGSPARSE_H
#define ARGSPARSE_H

namespace ArgsParse {
	extern char *in, *embed, *trian, *coords;
	char *const NOT_NEED = (char *) -1;
	extern bool backend;
	void parse_args(int argc, char *argv[]);
}
#endif				//ARGSPARSE_H
