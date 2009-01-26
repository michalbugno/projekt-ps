CFLAGS = -Wall -I/opt/local/include -L/opt/local/lib
LDFLAGS = -lpcap -lc -lconfig -lpthread -lstrophe -lexpat -lresolv -lssl

all: aids doc review.pdf

aids: main.c network.o system.o utils.o connection.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

network.o: network.c network.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

system.o: system.c system.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

connection.o: connection.c connection.h
	$(CC) $(CFLAGS) -c -o $@ $*.c

doc: network.c network.h system.c system.h main.c main.h
	doxygen doxygen.conf

review.pdf: review.tex
	texi2pdf -c -q review.tex
	make -C referat

clean:
	rm -rf *.o doc *.aux *.log *.toc
