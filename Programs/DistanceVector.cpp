#include "DistanceVector.h"

int portNum;
int ttl = 90;
int period = 30;
int splitHorizon = 0;
char *confFileName;
list<IP_Address> neighbors;
list<RecvAdvertisement> advertisementQueue;
IP_Address hostName;
struct timeval tv_main;
pthread_mutex_t sendLockMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t recvLockMutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t tllLockMutex  = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t sendCondThread = PTHREAD_COND_INITIALIZER;
pthread_cond_t recvCondThread = PTHREAD_COND_INITIALIZER;
//pthread_cond_t ttlCondThread  = PTHREAD_COND_INITIALIZER;

int sockFd;
int main(int argc, char * argv[]){
	//int sockFd;
	gettimeofday(&tv_main,NULL);
	pthread_t sendingThread;
	pthread_t receivingThread;
	pthread_t ttlUpdateThread;
	pthread_t updateThread;
	if(argc<7){
		cout<< "Parameters for executing the program"<<"\n";
		cout<< "1. Config File"<<"\n";
		cout<< "2. Port Number"<<"\n";
		cout<< "3. TTL in seconds"<<"\n";
		cout<< "4. Infinity values"<<"\n";
		cout<< "5. Advertisement Period in seconds"<<"\n";
		cout<< "6. yes/no Split Horizon"<<"\n";
		exit(0);
	}
	sockFd = socket(AF_INET,SOCK_DGRAM,0);
	confFileName = argv[1];
	portNum = atoi(argv[2]);
	ttl = atoi(argv[3]);
	infinity = atoi(argv[4]);
	period = atoi(argv[5]);
	if(strcasecmp(argv[6],"yes")==0){
		splitHorizon = 1;
	}else{
		splitHorizon = 0;
	}
	whoAmI();	
	HostAddr host_addr = labels[hostName];
	if(bind(sockFd,(sockaddr *)&host_addr,sizeof(host_addr))<0)
	{
		error("Cannot Bind\n");
	}	
	createGraph(confFileName);
	//constructRoutingTable();
	//updateRoutingTable();
	printGraph();
	DisplayRoutingTable();
	list<IP_Address>::iterator it;
	pthread_create(&updateThread,NULL,&updateGraph,NULL);
	pthread_create(&receivingThread,NULL,&receiveAvertisements,NULL);
   	pthread_create(&sendingThread,NULL,&sendAdvertisementsRoutine,NULL);
	pthread_create(&ttlUpdateThread,NULL,&ttlUpdate,NULL);
   	pthread_join(receivingThread,NULL);
   	pthread_join(sendingThread,NULL);
	pthread_join(ttlUpdateThread,NULL);
	return 0;
}


void DisplayRoutingTable(){
	printGraph();
	cout <<"\n"<< "Routing Table ";
	printTime();
	map<string,RouteEntry>::iterator it1;
	for(it1 = RoutingTable.begin();it1!=RoutingTable.end();++it1){
		RouteEntry temp = it1->second;
		cout.width(30);
		cout<<left << temp.destination <<"\t";
		cout.width(30);
		cout<<left << temp.nextHop <<"\t";
		cout.width(4);
		cout<<left <<temp.cost <<"\t";
		cout.width(4);
		cout<<left <<temp.TTL <<"\n";
	}
//	printGraph();
}

void DisplayAdvertisements(){	
	list<AdvertisementEntry> advers = getAdvertisements("blesmol.soic.indiana.edu");
	list<AdversisementEntry>::iterator it;
	cout << "Advertisements"<< "\n";
	for(it = advers.begin();it!=advers.end();++it){
		AdvertisementEntry temp= *it;
		cout<< temp.ipAddr <<"\t";
		cout<< temp.cost <<"\n";
	}
}

void* ttlUpdate(void *details){
	while(1){
	//	pthread_mutex_lock(&sendLockMutex);
		int isTTLExhausted = 0;
		map<string,RouteEntry>::iterator it;
		for(it = RoutingTable.begin();it!=RoutingTable.end();++it){
			RouteEntry routeEntry = it->second;
			if(routeEntry.destination.compare(hostName)!=0){
				routeEntry.TTL-=1;
			}	
			string dest = routeEntry.destination;
			RoutingTable[dest] = routeEntry;
			if(routeEntry.TTL==0){
				lostNode(dest);
				isTTLExhausted = 1;
			}
		}
		if(isTTLExhausted ==1){
		//	updateRoutingTable();	
			sendAdvertisements();
		}
	//	pthread_mutex_unlock(&sendLockMutex);
		sleep(1);
	}
	return NULL;
}

void sendAdvertisements(){	
	pthread_mutex_lock(&sendLockMutex);
	list<IP_Address>::iterator it;
	for(it = neighbors.begin();it!=neighbors.end();++it){
		string dest = *it;
	//	cout <<"\n\n"<<dest<<"\n";
		list<AdversisementEntry> adverToSend = getAdvertisements(dest);
		string adverString = AdvertisementToBuffer(adverToSend);
		char *buffer = (char*)malloc(sizeof(char) * 1000);
		bzero(buffer,1000);
		strncpy(buffer,adverString.c_str(),adverString.length());
		HostAddr neighAddr = labels[dest];
		socklen_t srvlen = sizeof(neighAddr);
	//	cout<<buffer;
		sendto(sockFd,buffer,1000,0,(struct sockaddr*)&neighAddr,srvlen);
	}

	
	pthread_mutex_unlock(&sendLockMutex);
}

void *sendAdvertisementsRoutine(void *sockFdPtr){
	while(1){
		DisplayRoutingTable();
		sendAdvertisements();
		sleep(period);
	}
	return NULL;
}


void *receiveAvertisements(void* sockFdPtr){
	HostAddr clientAddr;
	socklen_t srvlen = sizeof(clientAddr);
	char *buffer = (char*)malloc(sizeof(char)*1000);
	while(1){
		bzero(buffer,1000);
		recvfrom(sockFd,buffer,1000,0,(struct sockaddr*)&clientAddr,&srvlen);
		struct hostent *host = gethostbyaddr(&clientAddr.sin_addr,srvlen,AF_INET);
	//	cout<<"\n " << buffer<<"\n";
		string advertisementReceived = buffer;
		
		pthread_mutex_lock(&recvLockMutex);
		//parseAdvertisements(BufferToAdvertisement(advertisementRecieved),host->h_name);
		RecvAdvertisement advert;
		advert.hostName = host->h_name;
		advert.advertisement = advertisementReceived;
		advertisementQueue.push_back(advert);
		pthread_cond_signal(&recvCondThread);
		//DisplayRoutingTable();	
		pthread_mutex_unlock(&recvLockMutex);

	}	
	return NULL;
}

void *updateGraph(void *details){
	int isCostChanged = 0;
	while(1){
		if(advertisementQueue.empty()){
			pthread_mutex_lock(&recvLockMutex);
			updateRoutingTable(isCostChanged);
			isCostChanged = 0;
			pthread_cond_wait(&recvCondThread,&recvLockMutex);
			pthread_mutex_unlock(&recvLockMutex);
		}
		RecvAdvertisement advert = advertisementQueue.front();
		advertisementQueue.pop_front();
		int isChanged = parseAdvertisements(BufferToAdvertisement(advert.advertisement),advert.hostName);
		if(isChanged == 1){
			isCostChanged = 1;
		}
		
	}
}

void printTime(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	cout << tv.tv_sec-tv_main.tv_sec << ":" << tv.tv_usec-tv_main.tv_usec<<"\n";
}
