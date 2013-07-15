CC := gcc
CFLAGS := -Wall -g

all: libcspy.so 

clean:
	$(RM) *.o *~

libcspy.so: cspy.o
	$(CC) -shared -Wl,--export-dynamic -o $@ $^  -ldl

cspy.o: cspy.c
	$(CC) -c $(CFLAGS) -o $@ $^ -fPIC -dPIC

test: test.o
	$(CC) -o $@ $^ -ldl

.PHONY: all clean