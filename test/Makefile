CXX = g++
CFLAGS = -Wall -Wextra -O0 -g

all: test_hello test_pingpong test_wait

test_hello: hello.cpp
	$(CXX) $(CFLAGS) -I.. hello.cpp ../libgpp.a -nostartfiles -o hello

test_pingpong: pingpong.cpp
	$(CXX) $(CFLAGS) -I.. pingpong.cpp ../libgpp.a -nostartfiles -o pingpong

test_wait: wait.cpp
	$(CXX) $(CFLAGS) -I.. wait.cpp ../libgpp.a -nostartfiles -o wait

clean:
	rm -f *.o hello pingpong wait