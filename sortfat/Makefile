CC := gcc
PROGNAME := sortfat

all: $(PROGNAME)

clean:
	-$(RM) -rf *.o *.a *~

.PHONY: all  clean

$(PROGNAME): $(PROGNAME).o
	$(CC) -o $@ $<