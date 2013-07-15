CC := gcc
PROGNAME := codecfnames
CFLAGS += `pkg-config --cflags glib-2.0`
LDFLAGS += `pkg-config --libs glib-2.0`
all: $(PROGNAME)


clean:
	$(RM) *.o *~ semantic*

.PHONY:all clean

$(PROGNAME): $(PROGNAME).o
	$(CC) -o $@ $^ $(LDFLAGS)

