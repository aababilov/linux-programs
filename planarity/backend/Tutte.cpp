#include <cmath>

#include "Tutte.hpp"
#include "Matrix.hpp"

using namespace std;
using namespace Tutte;

double radius = 200;

typedef Matrix < double >MyMatrix;


void Drawer::Execute(Planarity::Graph::Storage & graph)
{
	int v = graph.nbrs.size();
	MyMatrix M(v, v), crds0(v, 2);
	vector < bool > marked(v);
	int extFace[] = { 0, graph.nbrs[0][0], graph.nbrs[0][1] };

	int len = 3;
	float angle = 2 * M_PI / len;
	//set the coords of FB's face, on a circle
	for (int i = 0; i < len; ++i) {
		crds0[extFace[i]][0] = radius * cos(i * angle);
		crds0[extFace[i]][1] = radius * sin(i * angle);
		marked[extFace[i]] = true;
	}
	for (int i = 0; i < v; ++i) {
		M[i][i] = 1;
		if (!marked[i]) {
			double d = -1.0 / graph.nbrs[i].size();
			for (vector < int >::iterator j =
			     graph.nbrs[i].begin(), ej =
			     graph.nbrs[i].end(); j != ej; ++j)
				M[i][*j] = d;
		}
	}

	MyMatrix inv, crds;
	inv.SetInverseOf(M);

	crds.SetProduct(inv, crds0);
	graph.coords.resize(v);
	for (int i = v - 1; i >= 0; --i) {
		graph.coords[i].x = crds[i][0] + radius + 8;
		graph.coords[i].y = crds[i][1] + radius + 8;
	}
}
