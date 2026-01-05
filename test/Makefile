CXX = g++
CFLAGS = -Wall -Wextra -O0 -g

all: test_hello test_pingpong

test_hello: hello.cpp
	$(CXX) $(CFLAGS) -I.. hello.cpp ../libgpp.a -nostartfiles -o hello

test_pingpong: pingpong.cpp
	$(CXX) $(CFLAGS) -I.. pingpong.cpp ../libgpp.a -nostartfiles -o pingpong

clean:
	rm -f *.o hello pingpong