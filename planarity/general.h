#ifndef GENERAL_H
#define GENERAL_H

#include <gtk/gtk.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

extern GtkWidget *mainwindow, *drawingarea,
	*polygon_dialog, *number_vtx, *triangulate;
extern char *backend;
void general_setup(int argc, char *argv[]);

#endif /*GENARAL_H*/
