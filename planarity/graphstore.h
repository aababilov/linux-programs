#ifndef GRAPHSTORE_H
#define GRAPHSTORE_H

#include <stdio.h>

typedef float coord_t;

enum { MAX_VERTICES = 256,
       MAX_EDGES = 6 * MAX_VERTICES
};

enum {E_OK, E_CANNOT_OPEN, E_BAD_FORMAT};

struct edge;

struct vertex {
	coord_t x, y;
	struct edge *first_e;
};

struct edge {
	struct vertex *v;
	struct edge *mirror, *next, *prev;
};

struct graph {
	int nv, ne;
	struct vertex v[MAX_VERTICES], *sel_vtx;
	struct edge e[MAX_EDGES], *free_e, *sel_edge;
	int saved;
	char *filename;
};

extern struct graph graph;

struct vertex *vertex_add(coord_t x, coord_t y);
struct vertex *vertex_find(coord_t x, coord_t y);
void vertex_delete(struct vertex *v);

struct edge *edge_add(struct vertex *u, struct vertex *v);
struct edge *edge_find(coord_t x, coord_t y);
void edge_delete(struct edge *e);

void graph_clear(void);
void graph_init(void);
int graph_save(void);
int graph_load(char *filename);
void graph_read_coords(FILE *f);
int graph_read_adjlist(FILE *f);
void graph_write_adjlist(FILE *f);
void graph_stretch(int width, int height);

#endif /*GRAPHSTORE_H*/
