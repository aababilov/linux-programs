#CPPFLAGS += $(shell pkg-config --cflags gtk+-2.0)
#LDFLAGS += $(shell pkg-config --libs gtk+-2.0)
OBJECTS = lcs.o
#HEADERS := $(shell ls *.hpp)
CFLAGS += -Wall

all: lcs

clean:
	$(RM) *.o *~*

.PHONY: all clean

lcs: $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJECTS): $(HEADERS)
