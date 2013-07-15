#ifndef PLANARITY_H
#define PLANARITY_H

#include <cstdio>
#include <vector>
#include <set>
#include <stdexcept>

namespace Planarity {

	class Embedder;
	class Triangulator;
	class Drawer;

	struct FloatPoint {
		double x, y;
	};

	class Graph {
	public:
		struct Storage {
			std::vector<std::vector<int> > nbrs;
			std::vector<FloatPoint> coords;
		private:
			friend class Graph;
			void Read(std::FILE * file);
			void ReadAdjList(std::FILE * file);
			void ReadAdjMatrix(std::FILE * file);
			void Write(std::FILE * file);
		};
		class BadData:public std::runtime_error {
		      public:
			BadData(const std::string & arg = "")
		      :    std::runtime_error(arg) {
		}};
		class NonPlanar:public std::runtime_error {
		      public:
			NonPlanar(const std::string & arg = "")
		      :    std::runtime_error(arg) {
		}};
	private:
		Storage graphStorage_;
		Embedder & embedder_;
		Triangulator & triangulator_;
		Drawer & drawer_;
		bool embedded_, triangulated_, drawn_, nonplanar_;
	public:
		Graph(Embedder & embedder,
		      Triangulator & triangulator, Drawer & drawer)
	      :    embedder_(embedder),
		    triangulator_(triangulator),
		    drawer_(drawer) {
		}
		void Read(std::FILE * file) {
			embedded_ = triangulated_ = drawn_ = false;
			graphStorage_.Read(file);
		}
		void Write(std::FILE * file) {
			graphStorage_.Write(file);
		}
		void Draw(std::FILE * file);

#define GEN_FILE_FUNCS(name, mode, dflt)		\
    void name(int file) {            	      \
      using namespace std;           			\
      name(fdopen(file, "r"));       			\
    }                                			\
    void name(const char *file) {    			\
      using namespace std;           			\
      if (file) {					             \
	FILE *f = fopen(file, mode);   		    	\
	if (!f)                                    \
    throw BadData(std::string("File not found: ") \
		  + file);                                   \
	name(f);			     \
	fclose(f);					\
      } else						\
	name(dflt);      				\
    }

		GEN_FILE_FUNCS(Read, "rt", stdin);
		GEN_FILE_FUNCS(Write, "w+t", stdout);
		GEN_FILE_FUNCS(Draw, "w+t", stdout);
		void Embed();
		void Triangulate();
	};

	class GraphAlgorithm {
	public:
		virtual void Execute(Graph::Storage & graph) = 0;
	};
	class Embedder:public GraphAlgorithm {
	};
	class Triangulator:public GraphAlgorithm {
	};
	class Drawer:public GraphAlgorithm {
	};

}
#endif				//PLANARITY_H
