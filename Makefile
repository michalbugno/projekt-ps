CFLAGS = -Wall -I/opt/local/include -L/opt/local/lib
LDFLAGS = -lpcap -lc -lconfig -lpthread

all: aids doc

aids: main.c network.o system.o analyzer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

network.o: network.c network.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

system.o: system.c system.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

analyzer.o: analyzer.c analyzer.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

doc: network.c network.h system.c system.h main.c main.h analyzer.c analyzer.h
	doxygen doxygen.conf

clean:
	rm -rf *.o doc
