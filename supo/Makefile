CC := gcc
PROGNAME := supo
EXENAME := $(PROGNAME)

all: $(EXENAME)

clean:
	-$(RM) *.o *~ semantic*

.PHONY: clean

$(EXENAME): $(PROGNAME).o
	$(CC) -o $@ $<

setall:
	chown root:root $(EXENAME)
	chmod +s $(EXENAME)