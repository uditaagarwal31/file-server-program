all: static shared debug server 

static:
	mkdir -p build
	mkdir -p build/lib
	mkdir -p build/lib/release
	mkdir -p build/objects
	g++ src/lib.cpp -c -I include -I lib
	ar rs libhashmap.a lib.o
	mv *.o build/objects
	mv *.a build/lib/release

shared:
	mkdir -p build
	mkdir -p build/lib
	mkdir -p build/lib/release
	mkdir -p build/objects
	g++ -fPIC src/lib.cpp -c -I include -I lib
	g++ -shared -o libhashmap.so lib.o
	mv *.o build/objects
	mv *.so build/lib/release

debug:
	mkdir -p build
	mkdir -p build/lib
	mkdir -p build/lib/release
	mkdir -p build/objects
	g++ -g -fPIC src/lib.cpp -c -I include -I lib
	g++ -shared -o libhashmap.so lib.o
	mv *.o build/objects
	mv *.so build/lib/release

clean:
	rm -rf build

server.o: src/bin/server.cpp
	g++ src/bin/server.cpp -c -I include -I lib

libhashmap.a:
	mkdir -p build
	mkdir -p build/lib
	mkdir -p build/lib/release
	mkdir -p build/objects
	g++ src/lib.cpp -c -I include -I lib
	ar rs libhashmap.a lib.o

server: server.o libhashmap.a
	g++ server.o -o server -lhashmap -L. -L lib/build/lib/release
	mkdir -p build/bin/release
	mkdir -p build/lib/release
	mkdir -p build/objects
	mv *.o build/objects
	mv *.a build/lib/release
	mv server build/bin/release

