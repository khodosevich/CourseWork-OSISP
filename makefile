CC = g++
CFLAGS = -std=c++11

all: portscanner

portscanner: main.cpp
	$(CC) $(CFLAGS) -o portscanner main.cpp

clean:
	rm -f portscanner
