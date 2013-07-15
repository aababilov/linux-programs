#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "general.h"
#include "graphstore.h"
#include "draw_mouse.h"

int dawidth, daheight;
static GdkPixmap *pixmap = NULL;
enum {r = 4};
static GdkGC *text_gc, *white_gc, *black_gc, *dash_gc, *sel_gc;
static PangoContext *context;
static PangoLayout *layout;
static GdkColor clBlack = {0}, clRed = {0xff0000},
	clWhite = {0xffffff};
static GdkPoint move_pos, down_pos;

static void reselect_vertex(struct vertex *v);
static void reselect_edge(struct edge *e);

static int min_of(int a, int b)
{
	return a < b ? a : b;
}

static int max_of(int a, int b)
{
	return a > b ? a : b;
}

#define MAKE_RECT(a, b) min_of((a)->x, (b)->x) - 1, min_of((a)->y, (b)->y) - 1, \
		abs((a)->x - (b)->x) + 2, abs((a)->y - (b)->y) + 2

static void draw_edge(struct edge *e)
{
	gdk_draw_line(pixmap,
		      (graph.sel_edge == e || graph.sel_edge == e->mirror) ?
		      sel_gc : black_gc,
		      e->mirror->v->x, e->mirror->v->y,
		      e->v->x, e->v->y);
}

static void draw_vertex(struct vertex *v)
{
	static char buf[8];
	gdk_draw_arc(pixmap, graph.sel_vtx == v ? sel_gc : black_gc, TRUE,
		     v->x - r, v->y - r, 2 * r, 2 * r,
		     0, 64 * 360);
	sprintf(buf, "%d", v - graph.v);
	pango_layout_set_text(layout, buf, -1);
	gdk_draw_layout(pixmap, text_gc,
			v->x + r + 1, v->y - r,	layout);
}

static void draw_all(void)
{
	int i;
	gdk_draw_rectangle(pixmap, white_gc,
			   TRUE, 0, 0, dawidth, daheight);
	for (i = 0; i < graph.nv; ++i) {
		struct edge *e, *first_e;
		struct vertex *v = &graph.v[i];
		first_e = e = graph.v[i].first_e;
		if (e) 
			do {
				if (e->v > v)
					draw_edge(e);
				e = e->next;
			} while (e != first_e);
		draw_vertex(v);
	}
}

void redraw_and_refresh(void)
{
	draw_all();
	gtk_widget_queue_draw(drawingarea);
}

static update_vertex(struct vertex *v)
{
	if (v) {
		draw_vertex(v);			
		gtk_widget_queue_draw_area(drawingarea,
					   v->x - r, v->y - r,
					   r * 2 + 100, 20);
	}			
}

static update_edge(struct edge *e)
{
	if (e) {
		draw_edge(e);
		gtk_widget_queue_draw_area(drawingarea,
					   MAKE_RECT(e->v, e->mirror->v));
	}			
}

static void reselect_vertex(struct vertex *v)
{
	if (graph.sel_vtx != v) {
		struct vertex *t = graph.sel_vtx;
		graph.sel_vtx = v;
		if (graph.sel_vtx)
			reselect_edge(NULL);
		update_vertex(t);
		update_vertex(graph.sel_vtx);
	}
	if (graph.sel_vtx) {
		move_pos.x = graph.sel_vtx->x;
		move_pos.y = graph.sel_vtx->y;
	}

}

static void reselect_edge(struct edge *e)
{
	if (graph.sel_edge != e) {
		struct edge *t = graph.sel_edge;
		graph.sel_edge = e;
		update_edge(t);
		update_edge(graph.sel_edge);
		if (graph.sel_edge)
			reselect_vertex(NULL);
	}
}

gboolean on_drawingarea_button_press_event(GtkWidget      *widget,
					   GdkEventButton *event,
					   gpointer        user_data)
{
	GdkRectangle update_rect;
	struct vertex *v;
	if (event->type != GDK_BUTTON_PRESS
	    && event->type != GDK_2BUTTON_PRESS)
		return TRUE;
	down_pos.x = event->x;
	down_pos.y = event->y;
	switch (event->type) {
	case GDK_BUTTON_PRESS:
		v = vertex_find(event->x, event->y);
		if (v) 
			reselect_vertex(v);
		else 
			reselect_edge(edge_find(event->x, event->y));
		break;
	case GDK_2BUTTON_PRESS:
		reselect_vertex(vertex_add(event->x, event->y));
		break;
	}
	return TRUE;
}

gboolean on_drawingarea_button_release_event(GtkWidget      *widget,
					     GdkEventButton *event,
					     gpointer        user_data)
{
	struct vertex *fin_vtx;
	GdkPoint sel_pos;
	if (!graph.sel_vtx || down_pos.x == event->x && down_pos.y == event->y)
		return TRUE;
	switch (event->button) {
	case 1: /* left - move the selected vertex */
		graph.sel_vtx->x = event->x;
		graph.sel_vtx->y = event->y;
		draw_all();
		gtk_widget_queue_draw(widget);
		break;
	case 3: /* right - add an edge */
		sel_pos.x = graph.sel_vtx->x;
		sel_pos.y = graph.sel_vtx->y;
		reselect_edge(edge_add(graph.sel_vtx, vertex_find(event->x, event->y)));
		gtk_widget_queue_draw_area(widget, MAKE_RECT(&sel_pos, event));
		break;
	}    
	return TRUE;
}

gboolean on_drawingarea_motion_notify_event(GtkWidget      *widget,
                                            GdkEventMotion *event,
                                            gpointer        user_data)
{
	int x, y;
	if (graph.sel_vtx && (event->state & GDK_BUTTON3_MASK)) {
		gdk_window_get_pointer(widget->window, &x, &y, NULL);
		gdk_draw_line(widget->window, dash_gc,
			      graph.sel_vtx->x, graph.sel_vtx->y,
			      move_pos.x, move_pos.y);
		gdk_draw_line(widget->window, dash_gc,
			      graph.sel_vtx->x, graph.sel_vtx->y, x, y);
		move_pos.x = x;
		move_pos.y = y;
	}
	return TRUE;
}

static void setup_gcs(void)
{
	GdkGCValues gcvals = {0};
	gcvals.foreground = clRed;
	text_gc = gdk_gc_new_with_values(pixmap, &gcvals, GDK_GC_FOREGROUND);
	gcvals.foreground = clBlack;
	black_gc = gdk_gc_new_with_values(pixmap, &gcvals, GDK_GC_FOREGROUND);
	gcvals.background = clBlack;
	gcvals.function = GDK_INVERT;
	gcvals.line_style = GDK_LINE_ON_OFF_DASH;
	dash_gc = gdk_gc_new_with_values(pixmap, &gcvals,
					 GDK_GC_FUNCTION |
					 GDK_GC_LINE_STYLE);
	gcvals.foreground = clWhite;
	white_gc = gdk_gc_new_with_values(pixmap, &gcvals,
					  GDK_GC_FOREGROUND);	
	sel_gc = text_gc;
}

gboolean on_drawingarea_configure_event(GtkWidget         *widget,
					GdkEventConfigure *event,
					gpointer           user_data)
{
	int isfirst = 0;	
	dawidth = event->width;
	daheight = event->height;
	if (pixmap) 
		g_object_unref (pixmap);		
	else
		isfirst = 1;
	pixmap = gdk_pixmap_new(widget->window, dawidth, daheight, -1);
	if (isfirst)
		setup_gcs();
	if (layout)
		g_object_unref(layout);	  	
	context = gtk_widget_get_pango_context(GTK_WIDGET(drawingarea));
	layout = pango_layout_new (context);

	draw_all();
	return TRUE;
}

gboolean on_drawingarea_expose_event(GtkWidget      *widget,
				     GdkEventExpose *event,
				     gpointer        user_data)
{
	gdk_draw_drawable (widget->window,
			   widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
			   pixmap,
			   event->area.x, event->area.y,
			   event->area.x, event->area.y,
			   event->area.width, event->area.height);
	return FALSE;
}

gboolean on_mainwindow_key_release_event(GtkWidget   *widget,
					 GdkEventKey *event,
					 gpointer     user_data)
{
	if (event->keyval == GDK_Delete ||
	    event->keyval == GDK_BackSpace) {
		printf("del\n");
		if (graph.sel_vtx) {
			vertex_delete(graph.sel_vtx);
			graph.sel_vtx = NULL;
		}
		else if (graph.sel_edge)
			edge_delete(graph.sel_edge);
		else
			return TRUE;
		redraw_and_refresh();
		printf("refr\n");
	}
	return TRUE;
}

void on_about_activate(GtkMenuItem     *menuitem,
		       gpointer         user_data)
{
}
