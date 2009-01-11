CFLAGS = -Wall -Iinclude -Llib
LDFLAGS = -lpcap -lc -lconfig -lpthread

all: aids doc

aids: main.c network.o system.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o aids main.c network.o system.o

network.o: network.c
	$(CC) $(CFLAGS) -c -o network.o network.c

system.o: system.c
	$(CC) $(CFLAGS) -c -o system.o system.c

doc:
	rm -rf doc
	doxygen doxygen.conf

clean:
	rm -rf *.o doc
