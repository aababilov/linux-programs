#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include "general.h"

GtkWidget *mainwindow, *drawingarea,
	*polygon_dialog, *number_vtx, *triangulate;
char *backend;
static char *dirs[2];
static char *lookup_file(char *filename);
	
static int create_mainwindow(void)
{
	GtkWidget *window;
	GladeXML *xml;
	GdkGCValues gcvals = {0};
	char *glade_file = lookup_file("render.glade");
	GtkWidget *vbox1;
	GParamSpec **props;
	guint n, i;
	
	if (!glade_file) {
		printf("Glade file not found!\n");
		_exit(0);
	}
	
	xml = glade_xml_new(glade_file, NULL, NULL);
	glade_xml_signal_autoconnect(xml);

	vbox1 = glade_xml_get_widget(xml, "vbox1");
	props = g_object_class_list_properties (G_OBJECT_GET_CLASS(vbox1),
		&n);
	for (i = 0; i < n; ++i)
		printf("%d: %s\n", i, props[i]->name);
	
	mainwindow = glade_xml_get_widget(xml, "mainwindow");
	drawingarea = glade_xml_get_widget(xml, "drawingarea");
	polygon_dialog = glade_xml_get_widget(xml, "polygon_dialog");
	number_vtx = glade_xml_get_widget(xml, "number_vtx");
	triangulate = glade_xml_get_widget(xml, "triangulate");

	free(glade_file);
	gtk_widget_show(mainwindow);
}

static char *lookup_file(char *filename)
{
	char *t;
	int i;
	if (g_path_is_absolute(filename)) 
		return g_file_test(filename, G_FILE_TEST_EXISTS)
			? strdup(filename) : NULL;
	else
		for (i = 0; i < ARRAY_SIZE(dirs); ++i) {
			t = g_build_filename(dirs[i], filename, NULL);
			if (g_file_test(t, G_FILE_TEST_EXISTS))
				return t;
			free(t);
		}
	return NULL;

}

static void free_dirs(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(dirs); ++i)
		free(dirs[i]);
}

static void setup_dirs(char *argv0)
{
	char *t;
	dirs[0] = g_get_current_dir();
	t = g_path_get_dirname(argv0);
	if (g_path_is_absolute(t))
		dirs[1] = t;
	else {
		dirs[1] = g_build_filename(dirs[0], t, NULL);
		free(t);
	}
}

static void usage(void) {
	printf("Usage: render [OPTIONS] [INPUT-FILE]\n"
	       "Available options:\n"
	       "\t-i, --input=file      \tFile for input\n"
	       "\t-V, --version         \tPrint version\n"
	       "\t-h, --help            \tShow this message\n");	       
}

static void parse_args(int argc, char *argv[])
{
	struct option long_options[] = {
		{"input", required_argument, NULL, 'i'},
		{"backend", required_argument, NULL, 'b'},
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, NULL, 0}
	};
	char c, *in = NULL, *t;
	while ((c =
		getopt_long(argc, argv, "i:vh",
			    long_options, NULL)) != -1) {
		switch (c) {
		case 'i':
			in = optarg;
			break;
		case 'b':
			backend = optarg;
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
	
	if (in) {
		in = lookup_file(in);
		if (in)
			graph_load(in);
	}
	if (!backend)
		backend = strcasecmp(argv[0] + strlen(argv[0]) - 4,
				     ".exe") == 0 ? "planarity.exe" : "planarity";
	backend = lookup_file(t = backend);
	if (!backend)
		printf("Backend not found: %s!\n", t);
	else if (!g_file_test(backend, G_FILE_TEST_IS_EXECUTABLE))
		printf("Backend not executable: %s\n", backend);
}

void general_setup(int argc, char *argv[])
{
	setup_dirs(argv[0]);
	parse_args(argc, argv);
	create_mainwindow();
	free_dirs();
}
