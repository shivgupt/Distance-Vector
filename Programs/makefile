CC = g++

all: DistanceVector clean

%.o: %.c
	$(CC) -c -o $@ $<

DistanceVector: DistanceVector.o RoutingTable.o Graph.o Helper.o
	$(CC) -o $@ DistanceVector.o RoutingTable.o Graph.o Helper.o -lpthread

clean:
	rm *.o
