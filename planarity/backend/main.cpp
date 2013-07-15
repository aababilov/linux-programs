#include <cstdio>

#include "Planarity.hpp"
#include "BoyerMyrvold.hpp"
#include "Ababilov.hpp"
#include "Tutte.hpp"
#include "ArgsParse.hpp"

using namespace std;

int main(int argc, char *argv[])
{
	ArgsParse::parse_args(argc, argv);

	BoyerMyrvold::Embedder emb;
	Ababilov::Triangulator trng;
	Tutte::Drawer drw;
	Planarity::Graph g(emb, trng, drw);

	try {
		g.Read(ArgsParse::in);
	}
	catch(Planarity::Graph::BadData & e) {
		fprintf(stderr, "%s\n", e.what());
		return 0;
	}
	try {
		g.Embed();
	}
	catch(Planarity::Graph::NonPlanar) {
		printf("non-planar\n");
		return 1;
	}
	
	if (ArgsParse::embed == ArgsParse::NOT_NEED &&
	    ArgsParse::trian == ArgsParse::NOT_NEED &&
	    ArgsParse::coords == ArgsParse::NOT_NEED) {
		printf("planar\n");
		return 0;
	}

	if (ArgsParse::embed != ArgsParse::NOT_NEED)
		g.Write(ArgsParse::embed);

	if (ArgsParse::trian != ArgsParse::NOT_NEED) {
		g.Triangulate();
		g.Write(ArgsParse::trian);
	}

	if (ArgsParse::coords != ArgsParse::NOT_NEED) {
		g.Draw(ArgsParse::coords);
	}
	return 0;
}
