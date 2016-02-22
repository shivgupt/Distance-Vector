#ifndef DISTANCEVECTOR_H_
#define DISTANCEVECTOR_H_
#include <stdio.h>
#include <iostream>
#include "Helper.h"
#include "Graph.h"
#include "RoutingTable.h"

typedef struct RecvAdvertisement{
string hostName;
string advertisement;
}RecvAdvertisement;

extern list<RecvAdvertisement> advertisementQueue;


void sendAdvertisements();
void *sendAdvertisementsRoutine(void*);
void *receiveAvertisements(void*);
void readConfigFile(FILE *fileName);
void* ttlUpdate(void *);
void DisplayRoutingTable(void);
void DisplayAdvertisements(void);
void* updateGraph(void*);
void printTime();
#endif /* DISTANCEVECTOR_H_ */
