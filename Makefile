CXX = g++
LD = ld
AR = ar
CFLAGS = -Wall -Wextra -O0 -g

PREFIX ?= /usr/local
LIBDIR = $(PREFIX)/lib
INCDIR = $(PREFIX)/include

libgpp.a: start.o proc.o tls.o gpp.o
	$(AR) rcs libgpp.a start.o proc.o tls.o gpp.o

start.o: start.s
	$(CXX) $(CFLAGS) -c start.s -o start.o

proc.o: proc.cpp
	$(CXX) $(CFLAGS) -c proc.cpp -o proc.o

tls.o: tls.cpp
	$(CXX) $(CFLAGS) -c tls.cpp -o tls.o

gpp.o: gpp.cpp
	$(CXX) $(CFLAGS) -c gpp.cpp -o gpp.o

clean:
	rm -f *.o *.a
	make -C test clean

test: test/test.sh libgpp.a
	./test/test.sh

install: libgpp.a
	mkdir -p $(LIBDIR)
	cp libgpp.a $(LIBDIR)/libgpp.a
	mkdir -p $(INCDIR)
	cp gpp.hpp $(INCDIR)/gpp.hpp

