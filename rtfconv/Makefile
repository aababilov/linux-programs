CC := gcc
PROGNAME := rtfconv
EXENAME := $(PROGNAME)

all: $(EXENAME)

clean:
	-$(RM) *.o *~ semantic*

.PHONY: clean

$(EXENAME): $(PROGNAME).o
	$(CC) -o $@ $<