#ifndef ROUTINGTABLE_H_
#define ROUTINGTABLE_H_
#include "Helper.h"
#include "Graph.h"



extern map<string,RouteEntry> RoutingTable;
void constructRoutingTable();
RouteEntry RelaxFunction(RouteEntry u,IP_Address v,int cost);
void updateRoutingTable(int);
int parseAdvertisements(list<AdversisementEntry> recvAdver,IP_Address from);
list<AdversisementEntry> getAdvertisements(IP_Address address);
string to_string(int cost);
list <AdversisementEntry>  BufferToAdvertisement(string ad);
string AdvertisementToBuffer(list<AdversisementEntry> adlist);
int isAdvertisementExist(IP_Address , list<AdversisementEntry>);
void resetTTL(IP_Address host,list<AdversisementEntry>);


#endif /* ROUTINGTABLE_H_ */
