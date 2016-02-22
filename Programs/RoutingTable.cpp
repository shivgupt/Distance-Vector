#include "RoutingTable.h"

map<string,RouteEntry> RoutingTable;
int infinity;



void constructRoutingTable(){
	list<IP_Address>::iterator iterator;
	for(iterator =	nodesList.begin() ;iterator!=nodesList.end();++iterator){
		string tempNode = *iterator;
		RouteEntry temp;
		temp.destination = tempNode;
		temp.nextHop = "NULL";
		temp.cost = infinity;
		temp.TTL = ttl;
		if(tempNode.compare(hostName)==0){
			temp.cost = 0;
			temp.nextHop = hostName;
		}
		RoutingTable[tempNode] = temp;

	}
}

RouteEntry RelaxFunction(RouteEntry u,IP_Address v,int cost){
	int dv = u.cost;
	int du = costMatrix[findIndex(hostName)][findIndex(v)];
	
	if(dv>du+cost){
		dv = du+cost;
		u.cost = dv;
		if(u.cost==1){
			u.nextHop = u.destination;
		}else{
			u.nextHop = v;
		}
	}
	return u;
}

void updateRoutingTable(int isChanged){
	map<string,RouteEntry>::iterator it;
	list<string>::iterator listIterator;
	int routingTableChanged = isChanged;
	int oldCost;
	IP_Address oldNextHop;
	for(it = RoutingTable.begin();it!=RoutingTable.end();++it){
		IP_Address nodeName;
		RouteEntry tempEntry = it->second;
		nodeName = tempEntry.destination;
		oldCost = tempEntry.cost;
		oldNextHop = tempEntry.nextHop;
		//tempEntry.cost = infinity;
		//tempEntry.nextHop = "NULL";
		//cout << nodeName <<"\n";
		int nodeIndex = findIndex(nodeName);
		for(listIterator = nodesList.begin();listIterator!=nodesList.end();++listIterator){
			string v = *listIterator;
			tempEntry = RelaxFunction(tempEntry,v,costMatrix[findIndex(v)][nodeIndex]);
		}
		RoutingTable[nodeName]  =tempEntry;
		if(tempEntry.cost != oldCost || tempEntry.nextHop.compare(oldNextHop)!=0)
		{
			routingTableChanged =1;
		}
	}
	if(routingTableChanged==1){
		sendAdvertisements();
		DisplayRoutingTable();
	}
}

int parseAdvertisements(list<AdversisementEntry> recvAdver,IP_Address from){
	list<AdversisementEntry>::iterator it;
	resetTTL(from,recvAdver);
	int isCostChanged = 0;
	for(it = recvAdver.begin();it!=recvAdver.end();++it){
		AdversisementEntry temp = *it;
		int isChanged = updateNode(temp,from);
		if(isChanged==1){
			isCostChanged = 1;
		}
	}
	return isCostChanged;
	//updateRoutingTable(isCostChanged);
	
}

list<AdversisementEntry> getAdvertisements(IP_Address address){
	list<AdversisementEntry> advertisement;
	map<string,RouteEntry>::iterator it;
	for(it = RoutingTable.begin();it!=RoutingTable.end();++it){
		RouteEntry tempEntry = it->second;
		if(splitHorizon==1 && address.compare(tempEntry.nextHop)==0){
			continue;
		}
		AdvertisementEntry temp;
		temp.ipAddr = tempEntry.destination;
		temp.cost = tempEntry.cost;
		advertisement.push_back(temp);
	}
	return advertisement;
}

string to_string(int cost)
{
	string result = "";
	stringstream outStream;
	outStream << cost;
	result = outStream.str();
	return result;
}

list <AdversisementEntry>  BufferToAdvertisement(string ad)
{
    int position, positionold;
    list <AdversisementEntry> parsedAd;
    position= positionold= 0;
    while(ad.find("\n",position+1) != string::npos )
    {
    	position = ad.find("\n",position+1);
        string line = ad.substr(positionold,position);
        positionold = position+1;
        int spacePosition = line.find(" ");
        AdversisementEntry temp;
        temp.ipAddr = line.substr(0,spacePosition);
        temp.cost = atoi(line.substr(spacePosition+1).c_str());
        parsedAd.push_back(temp);
    }

    return parsedAd;
}

string AdvertisementToBuffer(list<AdversisementEntry> adlist)
{
    list <AdversisementEntry> ::iterator it;
    string buffer = "";
    for(it = adlist.begin(); it != adlist.end(); it++)
    {
        AdversisementEntry temp = *it;
        buffer += temp.ipAddr;
        buffer += " ";
        buffer += to_string(temp.cost);
        buffer += "\n";
    }

    return buffer;
}

void resetTTL(IP_Address host,list<AdversisementEntry> recvAdver){
	RouteEntry temp = RoutingTable[host];
	temp.TTL = ttl;
	int isExist =0;
	RoutingTable[host] = temp;
	map<string,RouteEntry>::iterator it;
        for(it = RoutingTable.begin();it!=RoutingTable.end();++it){
                IP_Address nodeName;
                RouteEntry tempEntry = it->second;
                nodeName = tempEntry.nextHop;
                if(nodeName.compare(host)==0)
		{
			
			isExist = isAdvertisementExist(tempEntry.destination,recvAdver);
			if(isExist)
			{
				tempEntry.TTL = ttl;
				RoutingTable[tempEntry.destination]  =tempEntry;
			}
		}
        }

}

int isAdvertisementExist(IP_Address dest, list<AdversisementEntry> recvAdver){
	list<AdversisementEntry>::iterator it;
	for(it = recvAdver.begin();it!=recvAdver.end();++it){
		AdversisementEntry temp = *it;
		if(temp.ipAddr.compare(dest) == 0)
			return 1;
	}
	return 0;
}
