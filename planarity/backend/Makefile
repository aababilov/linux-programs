CC=gcc
CXX=g++
EXENAME=planarity
CHEADERFILE=appconst.h  graph.h  listcoll.h  stack.h
COBJFILES=graphPreprocess.o graphEmbed.o graphStructure.o stack.o listcoll.o
OBJFILES= $(COBJFILES) Ababilov.o BoyerMyrvold.o Tutte.o Planarity.o ArgsParse.o main.o

all: $(EXENAME)

clean:
	-$(RM) *.o *~ semantic*

Tutte.o: Matrix.hpp

%.o: %.cpp Planarity.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

%.o: %.hpp

$(COBJFILES): $(CHEADERFILES)

$(EXENAME): $(OBJFILES)
	$(CXX) $^ -o $@ $(LOADLIBES)