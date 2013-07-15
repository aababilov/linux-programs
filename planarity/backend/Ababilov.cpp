#include "Ababilov.hpp"

using namespace std;
using namespace Ababilov;

struct halfedge_t {
	int v;
	halfedge_t *prev, *next, *mirror;
};

namespace {
	int v, m, need_edges;
	vector<halfedge_t *> phalfedges, fnbr;
	vector<halfedge_t> halfedges;
	int ttl_halfedges;
}

void Triangulator::SetupStructures(Planarity::Graph::Storage & graph)
{
	v = graph.nbrs.size();
	m = 0;
	need_edges = (v - 2) * 3;
	phalfedges = vector<halfedge_t *>(v);
	fnbr = vector<halfedge_t *>(v);
	halfedges = vector<halfedge_t>(need_edges * 2);
	ttl_halfedges = 0;

	for (int a = 0; a < v; ++a) {
		halfedge_t *j;
		int b;

		for (j = fnbr[a]; j; j = j->next)
			phalfedges[j->v] = j;

		fnbr[a] = NULL;
		for (vector<int>::iterator j =
		     graph.nbrs[a].begin(), ej = graph.nbrs[a].end();
		     j != ej; ++j) {
			b = *j;
			++m;
			if (phalfedges[b]) {
				phalfedges[b]->prev = fnbr[a];
				fnbr[a] = phalfedges[b];
				phalfedges[b] = NULL;	//really this is not necessary!
			} else {
				halfedge_t *nthis =
				    &halfedges[ttl_halfedges], *nmirr =
				    nthis + 1;
				ttl_halfedges += 2;
				nthis->mirror = nmirr;
				nmirr->mirror = nthis;
				nthis->prev = fnbr[a];
				nmirr->next = fnbr[b];
				nthis->v = b;
				nmirr->v = a;
				fnbr[a] = nthis;
				fnbr[b] = nmirr;
			}
		}

		for (j = fnbr[a];; j = j->prev) {
			if (j->prev)
				j->prev->next = j;
			else {
				fnbr[a]->next = j;
				j->prev = fnbr[a];
				fnbr[a] = j;
				break;
			}
		}
	}

	m /= 2;
}

void Triangulator::Triangulate(Planarity::Graph::Storage & graph)
{
	vector<set<int> > incid(v);
	for (int a = 0; a < v; ++a) {		
		halfedge_t *i = fnbr[a];
		do {
			incid[a].insert(i->v);
			i = i->next;
		} while (i != fnbr[a]);
	}
	
	for (;;)
		for (int a = 0; a < v; ++a) {
			if (fnbr[a]->next != fnbr[a]) {
				halfedge_t *i = fnbr[a];
				do {
					if (!incid[i->v].count(i->next->v)) {
						halfedge_t *be =
							&halfedges[ttl_halfedges],
							*ce = be + 1;
						incid[i->v].insert(i->next->v);
						incid[i->next->v].insert(i->v);
						ttl_halfedges += 2;
						be->mirror = ce;
						ce->mirror = be;
						be->v = i->next->v;
						ce->v = i->v;
						be->prev = i->mirror->prev;
						ce->prev = i->next->mirror;
						be->next = be->prev->next;
						ce->next = ce->prev->next;
						be->prev->next = be;
						ce->prev->next = ce;
						be->next->prev = be;
						ce->next->prev = ce;
						++m;
						if (m == need_edges)
							return;
						if (fnbr[a]->next->next == fnbr[a])
							break;
					}
					i = i->next;
				}
				while (i != fnbr[a]);
			}
		}
	
}

void Triangulator::SaveStructures(Planarity::Graph::Storage & graph)
{
	for (int a = 0; a < v; ++a) {
		graph.nbrs[a].clear();
		halfedge_t *hlfe = fnbr[a];
		do {
			graph.nbrs[a].push_back(hlfe->v);
			hlfe = hlfe->next;
		} while (hlfe != fnbr[a]);
	}

}

		
void Triangulator::Execute(Planarity::Graph::Storage & graph)
{
	SetupStructures(graph);
	if (m == need_edges)
		return;
	Triangulate(graph);
	SaveStructures(graph);
}
