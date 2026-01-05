CXX = g++
CFLAGS = -Wall -Wextra -O0 -g

all: test_hello

test_hello: hello.cpp
	$(CXX) $(CFLAGS) -I.. hello.cpp ../libgpp.a -nostartfiles -o hello

clean:
	rm -f *.o hello