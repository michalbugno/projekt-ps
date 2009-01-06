CFLAGS = -Wall
LDFLAGS = -lpcap

all: aids

aids: main.c network.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o aids main.c network.o

network.o: network.c
	$(CC) $(CFLAGS) -c -o network.o network.c

clean:
	rm -rf *.o
