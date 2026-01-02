CXX = g++
LD = ld
CFLAGS = -Wall -Wextra -O0 -g

all: libgpp.o example.exe

libgpp.o: start.o proc.o tls.o
	$(LD) -r start.o proc.o tls.o -o libgpp.o

start.o: start.s
	$(CXX) $(CFLAGS) -c start.s -o start.o

proc.o: proc.cpp
	$(CXX) $(CFLAGS) -c proc.cpp -o proc.o

tls.o: tls.cpp
	$(CXX) $(CFLAGS) -c tls.cpp -o tls.o

example.exe: example/hello.cpp libgpp.o
	$(CXX) $(CFLAGS) example/hello.cpp libgpp.o -nostartfiles -o example.exe

clean:
	rm -f *.o example.exe example/hello.o

