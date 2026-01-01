CXX = g++
LD = ld
CFLAGS = -Wall -Wextra -O0 -g

all: libgpp.o example.exe

libgpp.o: start.o gpp.o
	$(LD) -r start.o gpp.o -o libgpp.o

start.o: start.S
	$(CXX) -O0 -c start.S -o start.o

gpp.o: gpp.cpp
	$(CXX) $(CFLAGS) -c gpp.cpp -o gpp.o

example.exe: example/hello.cpp libgpp.o
	$(CXX) $(CFLAGS) example/hello.cpp libgpp.o -nostartfiles -o example.exe

clean:
	rm -f *.o example.exe example/hello.o

