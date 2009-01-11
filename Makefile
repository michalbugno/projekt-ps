CFLAGS = -Wall
LDFLAGS = -lpcap -lc -lconfig -lpthread

all: aids doc

aids: main.c network.o system.o analyzer.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o aids main.c network.o system.o analyzer.o

network.o: network.c
	$(CC) $(CFLAGS) -c -o network.o network.c

system.o: system.c
	$(CC) $(CFLAGS) -c -o system.o system.c

analyzer.o: analyzer.c
	$(CC) $(CFLAGS) -c -o analyzer.o analyzer.c

doc:
	rm -rf doc
	doxygen doxygen.conf

clean:
	rm -rf *.o doc
