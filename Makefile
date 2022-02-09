CFLAGS=-O0 -ggdb -pedantic -Wall -std=c17 -D _POSIX_C_SOURCE=200809L
FUSEFLAGS=$(CFLAGS) `pkg-config fuse3 --cflags`
FUSELDFLAGS=`pkg-config fuse3 --libs`

SUDO ?=

CLING=$(SUDO) docker run --rm -it -v `pwd`:/mnt -w /mnt eldipa/cling cling %a

.PHONY: all
all: hanoifs

hanoifs: hanoi.o bitstack.o
	gcc $(FUSEFLAGS) bitstack.o hanoi.o hanoifs.c -o hanoifs $(FUSELDFLAGS)

hanoi.o: hanoi.c hanoi.h
	gcc $(CFLAGS) -c hanoi.c -o hanoi.o

bitstack.o: bitstack.c bitstack.h
	gcc $(CFLAGS) -c bitstack.c -o bitstack.o

libhanoi.so: hanoi.c hanoi.h
	gcc $(CFLAGS) -shared -fPIC -o libhanoi.so hanoi.c

libbitstack.so: bitstack.c bitstack.h
	gcc $(CFLAGS) -shared -fPIC -o libbitstack.so bitstack.c

.PHONY: test
test: libhanoi.so libbitstack.so
	byexample --ff --timeout=16 -l shell -l cpp -x-shebang="cpp:$(CLING)" *.md

.PHONY: pull_cling
pull_cling:
	$(SUDO) docker pull eldipa/cling

install_deps:
	$(SUDO) apt-get update && $(SUDO) apt-get install -y fuse3 libfuse3-dev libfuse3-3

install_test_deps: pull_cling
	pip install byexample

.PHONY: clean
clean:
	rm -f *.o *.so hanoifs
