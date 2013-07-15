#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "graphstore.h"
#include "general.h"

enum {EPSYLON = 4, EPSYLON_2 = EPSYLON * EPSYLON};
#define V_INDEX(av) (int)(av - graph.v)

struct graph graph;
static void add_half_edge(struct vertex *u, struct vertex *v);
static void remove_half_edge(struct edge *e);

static void remove_half_edge(struct edge *e)
{
	struct edge **pfirst_e = &e->mirror->v->first_e;
	if (e->next != e) {
		e->prev->next = e->next;
		e->next->prev = e->prev;
		if (*pfirst_e == e)
			*pfirst_e = e->next;			
	} else 
		*pfirst_e = NULL;
	e->next = graph.free_e;
	graph.free_e->next = e;	
}

struct vertex *vertex_add(coord_t x, coord_t y)
{
	int new_i = graph.nv;
	if (new_i >= MAX_VERTICES)
		return NULL;	
	graph.v[new_i].x = x;
	graph.v[new_i].y = y;
	graph.v[new_i].first_e = NULL;
	graph.saved = 0;
	return &graph.v[graph.nv++];
}

static coord_t dist_2(coord_t dx, coord_t dy)
{
	return dx * dx + dy * dy;
}

struct vertex *vertex_find(coord_t x, coord_t y)
{
	struct vertex *i;
	printf("vtx looking for\n");
	for (i = graph.v; i < graph.v + graph.nv; ++i)
		if (dist_2(i->x - x, i->y - y) < EPSYLON_2) {
			printf("vtx found: %d\n", V_INDEX(i));
			return i;
		}
	return NULL;
}

#define DIST(a, b) (sqrt(dist_2((a)->x - (b)->x, (a)->y - (b)->y)))
#define DOT_PRODUCT(s, a, b) (((a)->x - (s)->x) * ((b)->x - (s)->x) +	\
			      ((a)->y - (s)->y) * ((b)->y - (s)->y))
#define CROSS_PRODUCT(s, a, b) (((a)->x - (s)->x) * ((b)->y - (s)->y) - \
				((a)->y - (s)->y) * ((b)->x - (s)->x))

struct edge *edge_find(coord_t x, coord_t y)
{
	int i;
	struct vertex *v, t;
	t.x = x; t.y = y;
	for (v = graph.v + graph.nv; v >= graph.v; --v) {
		struct edge *e, *first_e;
		first_e = e = v->first_e;
		if (e) 
			do {
				if (fabs(CROSS_PRODUCT(e->v, v, &t)) <
				    DIST(e->v, v) * EPSYLON
				    && DOT_PRODUCT(e->v, &t, v) > 0
				    && DOT_PRODUCT(v, &t, e->v) > 0) {
					printf("found %d - %d\n", V_INDEX(v),
					       V_INDEX(e->v));
					return e;
				}
				e = e->next;
			} while (e != first_e);
	}
	return NULL;
}

void vertex_delete(struct vertex *v)
{
	struct edge *e, *first_e;
	int vi;
	if (!v)
		return;
	vi = V_INDEX(v);
	first_e = e = v->first_e;
	if (e) {
		do {
			remove_half_edge(e->mirror);
			e = e->next;
		} while (e != first_e);
		e->prev->next = graph.free_e;
		graph.free_e = e;
	}
	if (vi != graph.nv - 1) {
		first_e = e = v->first_e = graph.v[graph.nv - 1].first_e;
		if (e) {
			do {
				e->mirror->v = v;
				e = e->next;
			} while (e != first_e);
		}
		graph.v[vi] = graph.v[graph.nv - 1];
	}
	graph.v[graph.nv - 1].first_e = NULL;
	graph.saved = 0;
	--graph.nv;
	printf("vtx removed\n");	
}

static void add_half_edge(struct vertex *u, struct vertex *v)
{
	register struct edge *e = graph.free_e;
	int ui = V_INDEX(u);
	graph.free_e = e->next;
	e->v = v;
	if (u->first_e) {
		e->next = u->first_e;
		e->prev = u->first_e->prev;
		u->first_e->prev->next = e;
		u->first_e->prev = e;
	} else {
		e->next = e;
		e->prev = e;
	}
	u->first_e = e;
}

struct edge *edge_add(struct vertex *u, struct vertex *v)
{
	struct edge *e, *first_e;
	int vi;
	
	if (graph.ne >= MAX_EDGES || u == v || !u || !v)
		return NULL;
	vi = V_INDEX(v);
	first_e = e = v->first_e;
	if (e) 
		do {
			if (e->v == u)
				return e;
			e = e->next;
		} while (e != first_e);
	
	add_half_edge(u, v);
	add_half_edge(v, u);
	v->first_e->mirror = u->first_e;
	u->first_e->mirror = v->first_e;
	++graph.ne;
	graph.saved = 0;
	return v->first_e;
}

void edge_delete(struct edge *e)
{
	if (!e)
		return;
	remove_half_edge(e->mirror);
	remove_half_edge(e);
	graph.saved = 0;
	--graph.ne;
}

void graph_clear(void)
{
	struct vertex *v;
	for (v = graph.v + graph.nv - 1; v >= graph.v; --v)
		if (v->first_e) {
			v->first_e->next = graph.free_e;
			graph.free_e = v->first_e;
			v->first_e = NULL;
		}
	graph.sel_vtx = NULL;
	graph.sel_edge = NULL;
 	graph.saved = 1;
	graph.nv = graph.ne = 0;
}

void graph_init(void)
{
	int i;
	for (i = 1; i < MAX_EDGES; ++i)
		graph.e[i - 1].next = &graph.e[i];
	graph.free_e = &graph.e[0];
	graph.saved = 1;
	graph.nv = graph.ne = 0;
}

int graph_save(void)
{       
	int i;
	FILE *f = fopen(graph.filename, "w+t");
	if (!f)
		return E_CANNOT_OPEN;
	graph_write_adjlist(f);
	for (i = 0; i < graph.nv; ++i)
		fprintf(f, "%d: %d %d\n", i, (int)graph.v[i].x, (int)graph.v[i].y);
	fclose(f);
	graph.saved = 1;
	return E_OK;
}

void graph_write_adjlist(FILE *f)
{
	struct vertex *v;
	fprintf(f, "N=%d\n", graph.nv);
	for (v = graph.v; v < graph.v + graph.nv; ++v) {
		struct edge *e, *first_e;
		first_e = e = v->first_e;
		fprintf(f, "%d:", V_INDEX(v));
		if (e) 
			do {
				fprintf(f, " %d", V_INDEX(e->v));
				e = e->next;
			} while (e != first_e);
		fprintf(f, " -1\n");
	}
}

int graph_read_adjlist(FILE *f)
{
	int n = -10, i;
	if (fscanf(f, "N=%d", &n) != 1 || n < 1) 
		return E_BAD_FORMAT;	
	graph_clear();
	graph.nv = n;
	graph.saved = 1;
	for (i = 0; i < n; ++i) {
		while (fgetc(f) != ':')
			if (feof(f))
				return E_BAD_FORMAT;
		for (;;) {
			int j;
			if (fscanf(f, "%d", &j) != 1)
				return E_BAD_FORMAT;
			if (j == -1)
				break;
			if (j < i)
				edge_add(&graph.v[i], &graph.v[j]);
		}
	}
	return E_OK;
}
	
int graph_load(char *filename)
{
	FILE *f = fopen(filename, "rt");
	if (!f)
		return E_CANNOT_OPEN;
	if (graph.filename != filename) {
		if (graph.filename)
			free(graph.filename);
		graph.filename = filename;
	}
	if (graph_read_adjlist(f) != E_OK)
		return E_BAD_FORMAT;
	graph_read_coords(f);
	graph.saved = 1;
	fclose(f);
	return E_OK;
}

void graph_read_coords(FILE *f)
{
	int i;
	for (i = 0; i < graph.nv; ++i) {
		float x, y;
		char c;
		int j;
		while ((c = fgetc(f)) != ':') 
			if (feof(f))
				return;
		j = fscanf(f, "%f %f", &x, &y);
		printf("%d: %f %f\n", i, x, y);
		graph.v[i].x = x;
		graph.v[i].y = y;
	}
	graph.saved = 0;
}

void graph_stretch(int width, int height)
{
#define SET_MIN_MAX(c) do { if (v->c < min##c) min##c = v->c;	\
		if (v->c > max##c) max##c = v->c; } while (0)
#define CORRECT_MIN_MAX(c) if (max##c  - min##c < 4) max##c += 4
	struct vertex *v;
	float minx = 1e6, maxx = -1e6,
		miny = 1e6, maxy = -1e6,
		cminx = 0.1 * width, cminy = 0.1 * height,
		scalex, scaley;
	for (v = graph.v + graph.nv - 1; v >= graph.v; --v) {
		SET_MIN_MAX(x);
		SET_MIN_MAX(y);
	}
	CORRECT_MIN_MAX(x);
	CORRECT_MIN_MAX(y);
	scalex = 0.8 * width / (maxx - minx);
	scaley = 0.8 * height / (maxy - miny);
#define SET_COORD(c) v->c = (v->c - min##c) * scale##c + cmin##c
	for (v = graph.v + graph.nv - 1; v >= graph.v; --v) {
		SET_COORD(x);
		SET_COORD(y);
	}
}
