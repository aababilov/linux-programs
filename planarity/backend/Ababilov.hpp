#ifndef ABABILOV_H
#define ABABILOV_H

#include "Planarity.hpp"

namespace Ababilov {

	class Triangulator:public Planarity::Triangulator {
		virtual void Execute(Planarity::Graph::Storage & graph);
		static void SetupStructures(Planarity::Graph::Storage & graph);
		static void Triangulate(Planarity::Graph::Storage & graph);
		static void SaveStructures(Planarity::Graph::Storage & graph);
	};

}
#endif				//ABABILOV_H
