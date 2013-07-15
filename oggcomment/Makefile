CC=gcc
OBJFILES=main.o
LIBS=-logg -lvorbis
EXEC=oggcomment

all: $(EXEC)
clean:
	-rm -f *.o *.a *~ 

$(EXEC): $(OBJFILES) 
	$(CC) -o $@ $< $(LIBS)
