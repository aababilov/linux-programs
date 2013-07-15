#include <exception>
#include "Planarity.hpp"

using namespace std;
using namespace Planarity;

#define BAD_NUMBER_VERTICES throw BadData          \
 ("Error in input file: bad number of vertices")

void Graph::Storage::ReadAdjMatrix(FILE * file)
{
	int n;
	if (fscanf(file, "%d", &n) != 1 || n < 1)
		BAD_NUMBER_VERTICES;
	nbrs.resize(n);
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < n; ++j) {
			int flag = 0;
			if (fscanf(file, "%1d", &flag) != 1)
				throw BadData("Corrupted matrix");
			if (flag && j < i) {
				nbrs[i].push_back(j);
				nbrs[j].push_back(i);
			}
		}
}

void Graph::Storage::ReadAdjList(std::FILE * file)
{
	int n;
	if (fscanf(file, "N=%d", &n) != 1 || n < 1)
		BAD_NUMBER_VERTICES;
	nbrs.resize(n);
	for (int i = 0; i < n; ++i) {
		while (fgetc(file) != ':')
			if (feof(file))
				throw BadData("Incomplete file");
		for (;;) {
			int j;
			if (fscanf(file, "%d", &j) != 1)
				throw BadData("Incomplete file");
			if (j == -1)
				break;
			if (j < i) {
				nbrs[i].push_back(j);
				nbrs[j].push_back(i);
			}
		}
	}
}

void Graph::Storage::Read(std::FILE * file)
{
	char ch = fgetc(file);
	if (feof(file))
		throw BadData("Empty file");
	ungetc(ch, file);
	nbrs.clear();
	if (ch == 'N')
		ReadAdjList(file);
	else
		ReadAdjMatrix(file);

	int n = nbrs.size();
	vector < bool > marked(n);
	vector < int >q(n);
	int sq = 0, i = 0;
	for (int k = 0; k < n; ++k)
		if (!marked[k]) {
			if (k != 0) {
				nbrs[0].push_back(k);
				nbrs[k].push_back(0);
			}
			marked[q[sq++] = k] = true;
			for (; i < sq; ++i)
				for (vector < int >::iterator j =
				     nbrs[q[i]].begin(), ej =
				     nbrs[q[i]].end(); j != ej; ++j)
					if (!marked[*j])
						marked[q[sq++] = *j] =
						    true;
		}
}

void Graph::Storage::Write(FILE * file)
{
	int n = nbrs.size();
	fprintf(file, "N=%d\n", n);
	for (int i = 0; i < n; ++i, fprintf(file, " -1\n")) {
		fprintf(file, "%d:", i);
		for (vector < int >::iterator j = nbrs[i].begin(), ej =
		     nbrs[i].end(); j != ej; ++j)
			fprintf(file, " %d", (int) *j);
	}
}

void Graph::Embed()
{
	if (!embedded_) {
		embedded_ = nonplanar_ = true;
		embedder_.Execute(graphStorage_);
		nonplanar_ = false;
	} else if (nonplanar_)
		throw NonPlanar();
}

void Graph::Triangulate()
{
	if (!triangulated_) {
		Embed();
		triangulator_.Execute(graphStorage_);
		triangulated_ = true;
	}
}

void Graph::Draw(FILE * file)
{
	if (!drawn_) {
		Triangulate();
		drawer_.Execute(graphStorage_);
		drawn_ = true;
	}
	for (int i = 0, v = graphStorage_.nbrs.size(); i < v; ++i)
		fprintf(file, "%d: %.5f %.5f\n", i,
			(float) graphStorage_.coords[i].x,
			(float) graphStorage_.coords[i].y);	
}
