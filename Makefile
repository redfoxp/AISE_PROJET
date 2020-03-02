CC = gcc
AR = ar

all :

setup :
	mkdir -p ./build/lib
	mkdir -p ./build/include
	cp -r include ./build/

lib : setup lib/my_malloc.c
	$(CC) -c -fPIC -o build/lib/my_malloc.o lib/my_malloc.c
	$(CC) -shared -o build/lib/libmy_malloc.so build/lib/my_malloc.o
	$(AR) rcs build/lib/libmy_malloc.a build/lib/my_malloc.o

test : setup main.c lib
	$(CC) -I./build/include	-L./build/lib/ main.c -lmy_malloc -o build/test
	LD_LIBRARY_PATH=./build/lib ./build/test

clean :
	rm -rf build/* 