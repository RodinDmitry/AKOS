CC=g++
CFLAGS=-c
LDFLAGS=-lpthread
OBJECTS=client.o server.o
TARGETS=server client
all: $(OBJECTS) $(TARGETS)

%.o: %.cpp
	$(CC) $(CFLAGS) $^ 
%: %.o
	$(CC) $^ $(LDFLAGS)  -o $@
clear:
	rm $(OBJECTS)

