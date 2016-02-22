#include "Helper.h"

// print errors
void error(const char* msg){
	perror(msg);
	exit(0);
}

HostAddr *getHostIp(IP_Address name){
	struct hostent *host = gethostbyname(name.c_str());
	HostAddr *host_addr = (HostAddr*)malloc(sizeof(HostAddr));
	if(host==NULL){error("No such hosts");}
	bzero((char*)host_addr,sizeof(HostAddr));
	host_addr->sin_family = AF_INET;
	host_addr->sin_port = htons(portNum);
	bcopy((char*)host->h_addr,(char*)&host_addr->sin_addr.s_addr, host->h_length);
	return host_addr;
}
