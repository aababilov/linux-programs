#ifndef BOYERMYRVOLD_H
#define BOYERMYRVOLD_H

#include "Planarity.hpp"

namespace BoyerMyrvold {

	class Embedder:public Planarity::Embedder {
		virtual void Execute(Planarity::Graph::Storage & graph);
	};

}
#endif				//BOYERMYRVOLD_H
