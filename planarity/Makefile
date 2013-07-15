CC := gcc 
CFLAGS := $(shell pkg-config --cflags gtk+-2.0 libglade-2.0)
LDFLAGS :=$(shell pkg-config --libs gtk+-2.0 libglade-2.0) -Wl,--export-dynamic
LDFLAGS += -Wl,--export-all-symbols,--kill-at
OBJFILES := main.o draw_mouse.o graphstore.o filemenu.o general.o

all: render

clean:
	-$(RM) *.o *~ semantic*

render: $(OBJFILES)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

%.o: %.c %.h graphstore.h
