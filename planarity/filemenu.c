#include <stdio.h>
#include <math.h>
#include <gtk/gtk.h>

#include "general.h"
#include "graphstore.h"
#include "draw_mouse.h"

static GtkWidget *dialog;
#define WARN_AND_RET(w) do {g_warning(w); return;}while (0)

static void setup_cur_dir(void)
{
	gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER (dialog), TRUE);
	if (graph.filename) {
		char *dirname = g_path_get_dirname(graph.filename);
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog), dirname);
		g_free(dirname);
	}
}

static void save_as(void)
{
	dialog = gtk_file_chooser_dialog_new("Save as",
					     GTK_WINDOW(mainwindow),
					     GTK_FILE_CHOOSER_ACTION_SAVE,
					     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					     GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
					     NULL);
	setup_cur_dir();	
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		graph.filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER (dialog));
		graph_save();
	}
	gtk_widget_destroy (dialog);
}

static void check_is_saved(void)
{
	if (!graph.saved) {
		int must_save;
		dialog = gtk_message_dialog_new(GTK_WINDOW(mainwindow),
						GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_MESSAGE_QUESTION,
						GTK_BUTTONS_YES_NO,
						"Save this graph?");
		must_save = gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_YES;
		gtk_widget_destroy(dialog);
		if (must_save)
			save_as();
	}
}

void on_new_activate(GtkMenuItem     *menuitem,
		     gpointer         user_data)
{
	printf("new \n");
	check_is_saved();
	graph_clear();
	redraw_and_refresh();
}

void on_open_activate(GtkMenuItem     *menuitem,
		      gpointer         user_data)
{
	check_is_saved();
	dialog = gtk_file_chooser_dialog_new("Open",
					     GTK_WINDOW(mainwindow),
					     GTK_FILE_CHOOSER_ACTION_OPEN,
					     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					     NULL);
	setup_cur_dir();
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
		graph_load(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog)));
		redraw_and_refresh();
	}
	gtk_widget_destroy (dialog);
}

void on_save_activate(GtkMenuItem     *menuitem,
		      gpointer         user_data)
{
	if (graph.filename)
		graph_save();
	else
		save_as();
}

void on_save_as_activate(GtkMenuItem     *menuitem,
			 gpointer         user_data)
{
	save_as();
}

gboolean on_mainwindow_delete_event(GtkWidget *widget,
				      GdkEvent  *event,
				      gpointer   data )
{
	check_is_saved();
	return FALSE;
}

static void show_dlg(GtkMessageType type, const char *message)
{
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(mainwindow),
						   GTK_DIALOG_DESTROY_WITH_PARENT,
						   type,
						   GTK_BUTTONS_CLOSE,
						   message, NULL);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void on_polygon_activate(GtkMenuItem     *menuitem,
			 gpointer         user_data)
{
	check_is_saved();
	if (gtk_dialog_run(GTK_DIALOG(polygon_dialog)) == GTK_RESPONSE_OK) {
		int i, w2 = dawidth * 0.4, h2 = daheight * 0.4,
			last = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(number_vtx));
		graph_clear();		
		for (i = 0; i < last; ++i)
			vertex_add(w2 * cos(M_PI * 2 * i / last) + dawidth / 2,
				   h2 * sin(M_PI * 2 * i / last) + daheight / 2);
		for (i = 1; i < last; ++i)
			edge_add(&graph.v[i - 1], &graph.v[i]);
		edge_add(&graph.v[0], &graph.v[last - 1]);
		redraw_and_refresh();
	}
	gtk_widget_hide(polygon_dialog);	
}

void on_clear_activate(GtkMenuItem     *menuitem,
		       gpointer         user_data)
{
	check_is_saved();
	graph_clear();
	redraw_and_refresh();
}

void on_backend_activate(GtkMenuItem     *menuitem,
			 gpointer         user_data)
{
	gint in, out;
	FILE *fin, *fout;
	char *args[] = {backend, "--coordinates", NULL, NULL, NULL};
	int trian = menuitem == (GtkMenuItem*)triangulate;   
	GPid pid;
	printf("%p %p\n", menuitem, triangulate);
	if (trian) 
		args[2] = "--triangulate";

	if (!graph.nv) {		
		show_dlg(GTK_MESSAGE_ERROR, "Empty graph!");
		return;
	}
	if (!g_spawn_async_with_pipes(NULL, args, NULL, 0, NULL, NULL,
				      &pid, &in, &out, NULL, NULL)) {
		show_dlg(GTK_MESSAGE_ERROR, "Cannot use backend!");
		return;
	}
	fin = fdopen(in, "wt");
	fout = fdopen(out, "rt");
	graph_write_adjlist(fin);
	fclose(fin);
	if (getc(fout) == 'n')
		show_dlg(GTK_MESSAGE_WARNING, "Graph is not planar!");
	ungetc('N', fout);
	if (trian)
		graph_read_adjlist(fout);      
	graph_read_coords(fout);
	graph_stretch(dawidth, daheight);
	fclose(fout);

	g_spawn_close_pid(pid);
	redraw_and_refresh();
}

