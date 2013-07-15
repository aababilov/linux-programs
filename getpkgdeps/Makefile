PROGNAME := getpkgdeps
OBJFILES := main.o getlist.o getlist2.o

all: $(PROGNAME)

clean:
	$(RM) *.o *~ semantic.cache

$(PROGNAME): $(OBJFILES)
	$(CXX) -o $@ $^

$(OBJFILES): *.hpp