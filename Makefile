CFLAGS=-O0 -ggdb -pedantic -Wall -std=c17 -D _POSIX_C_SOURCE=200809L
FUSEFLAGS=$(CFLAGS) `pkg-config fuse3 --cflags --libs`

CLING=docker run --rm -it -v `pwd`:/mnt -w /mnt eldipa/cling cling %a

all: hanoilib.o bitstack.o
	gcc $(FUSEFLAGS) bitstack.o hanoilib.o hanoifs.c -o hanoifs

hanoilib.o: hanoilib.c hanoilib.h
	gcc $(CFLAGS) -c hanoilib.c -o hanoilib.o

bitstack.o: bitstack.c bitstack.h
	gcc $(CFLAGS) -c bitstack.c -o bitstack.o

libhanoilib.so: hanoilib.c hanoilib.h
	gcc $(CFLAGS) -shared -fPIC -o libhanoilib.so hanoilib.c

libbitstack.so: bitstack.c bitstack.h
	gcc $(CFLAGS) -shared -fPIC -o libbitstack.so bitstack.c

test: libhanoilib.so libbitstack.so
	byexample --timeout=8 -l cpp -x-shebang="cpp:$(CLING)" *.md

pull_cling:
	docker pull eldipa/cling

clean:
	rm -f *.o *.so hanoifs
