#include "BoyerMyrvold.hpp"
#include "graph.h"

using namespace std;
using namespace BoyerMyrvold;

void Embedder::Execute(Planarity::Graph::Storage & graph)
{
	graphP g = gp_New();
	int n = graph.nbrs.size();
	gp_InitGraph(g, n);
	for (int i = 0; i < n; ++i)
		for (vector < int >::iterator j =
		     graph.nbrs[i].begin(), ej = graph.nbrs[i].end();
		     j != ej; ++j)
			if (i < *j)
				gp_AddEdge(g, i, 0, *j, 0);
	if (gp_Embed(g, EMBEDFLAGS_PLANAR) != OK)
		throw Planarity::Graph::NonPlanar();
	gp_SortVertices(g);
	graph.nbrs.clear();
	graph.nbrs.resize(n);
	for (int i = 0; i < n; ++i)
		for (int j = g->G[i].link[1]; j >= g->N;
		     j = g->G[j].link[1])
			graph.nbrs[i].push_back(g->G[j].v);
}
