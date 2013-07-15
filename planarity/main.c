#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <gtk/gtk.h>

#include "general.h"
#include "graphstore.h"

#ifdef WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
	gtk_init (&argc, &argv);
#ifdef WIN32
//	FreeConsole();
#endif
	graph_init();
	general_setup(argc, argv);
	
	gtk_main();
	return 0;
}
