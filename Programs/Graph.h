#ifndef GRAPH_H_
#define GRAPH_H_
#include <iostream>
#include <list>
#include <map>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include "Helper.h"


typedef struct RouteEntry{
        IP_Address destination;
        IP_Address nextHop;
        int cost;
        unsigned short TTL;
}RouteEntry;

extern map<string,RouteEntry> RoutingTable;
extern map<string,HostAddr> labels;
extern list<IP_Address> nodesList;
extern int numberOfNodes;
extern list<IP_Address> neighbors;
extern IP_Address hostName;
extern int **costMatrix;
extern void DisplayRoutingTable(void);
void createGraph(char *file);
void lostNode(IP_Address host);
void printGraph(void);
void initializeSingleSourceGraph();
int findIndex(IP_Address host);
int updateNode(AdversisementEntry tempNode,IP_Address host);
void sendAdvertisements(void);
void constructRoutingTable(void);
void updateRoutingTable(int);
void whoAmI(void);
#endif /* GRAPH_H_ */
