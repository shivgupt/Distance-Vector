
#ifndef HELPER_H_
#define HELPER_H_
#include <string>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>
#include <netdb.h>
#include <sstream>
#include <arpa/inet.h>
#include <pthread.h>
using namespace std;


extern int infinity;
extern int ttl;
extern int period;
extern int splitHorizon;
extern int portNum;
typedef string IP_Address;
typedef struct sockaddr_in HostAddr;
typedef struct AdversisementEntry{
	IP_Address ipAddr;
	int cost;
}AdvertisementEntry;

typedef struct nodeDetails{
	IP_Address nextHop;
	int cost;
}nodeDetails;

typedef struct receiverDetails{
	IP_Address host;
	HostAddr recvFrm;
	int sockFd;
}receiverDetails;


typedef struct Advertisements{
	AdvertisementEntry Adversisement[100];
}Advertisements;

void error(const char* msg);
HostAddr *getHostIp(IP_Address name);
extern Advertisements advertisement;

#endif /* HELPER_H_ */
