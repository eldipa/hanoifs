CFLAGS=-O0 -ggdb -pedantic -Wall -std=c17 -D _POSIX_C_SOURCE=200809L
FUSEFLAGS=$(CFLAGS) `pkg-config fuse3 --cflags --libs`

CLING=docker run --rm -it -v `pwd`:/mnt -w /mnt eldipa/cling cling %a

all: hanoi.o bitstack.o
	gcc $(FUSEFLAGS) bitstack.o hanoi.o hanoifs.c -o hanoifs

hanoi.o: hanoi.c hanoi.h
	gcc $(CFLAGS) -c hanoi.c -o hanoi.o

bitstack.o: bitstack.c bitstack.h
	gcc $(CFLAGS) -c bitstack.c -o bitstack.o

libhanoi.so: hanoi.c hanoi.h
	gcc $(CFLAGS) -shared -fPIC -o libhanoi.so hanoi.c

libbitstack.so: bitstack.c bitstack.h
	gcc $(CFLAGS) -shared -fPIC -o libbitstack.so bitstack.c

test: libhanoi.so libbitstack.so
	byexample --ff --timeout=8 -l shell -l cpp -x-shebang="cpp:$(CLING)" *.md

pull_cling:
	docker pull eldipa/cling

clean:
	rm -f *.o *.so hanoifs
