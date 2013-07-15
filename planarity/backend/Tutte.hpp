#ifndef TUTTE_H
#define TUTTE_H

#include "Planarity.hpp"

namespace Tutte {

	class Drawer:public Planarity::Drawer {
		virtual void Execute(Planarity::Graph::Storage & graph);
	};

}
#endif				//TUTTE_H
