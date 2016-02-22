#include "Graph.h"
map<string,HostAddr> labels;
int numberOfNodes;
list<IP_Address> nodesList;
string currentHostName;
int **costMatrix;


void whoAmI(){

	char *hostname = (char*)calloc(sizeof(char),1024);
	gethostname(hostname,1024);
	//cout << hostname;
	//rootnode.name="bandicoot.soic.indiana.edu";
	hostName =  hostname;
	labels[hostName] = *(getHostIp(hostName));
	nodesList.push_back(hostName);
}

void createGraph(char *file){
	numberOfNodes = 1;
	fstream ifile;
	string line, nodeName;
	int position;
	ifile.open(file,ios::in);
	if (!ifile)
	{
		cout << "No config file found!! \n Program Terminating";
		exit(0);
	}
	else
	{
		ifile.unsetf(ios_base::skipws);
		while (getline(ifile,line))
		{
			//			cout << line<<"\n";
			if(line.empty()){continue;}
			numberOfNodes++;
			position = line.find(" ");
			nodeName = line.substr(0,position);
			nodesList.push_back(nodeName);
			labels[nodeName] = *(getHostIp(nodeName));
			if(strcasecmp(line.substr(position+1).c_str(),"yes")==0){
				neighbors.push_back(nodeName);
			}
		}
	}
	initializeSingleSourceGraph();
}

void initializeSingleSourceGraph(){
	int indexRow,indexColumn;
	costMatrix = (int**)malloc(sizeof(int*)*numberOfNodes);
	list<IP_Address>:: iterator colIterator,rowIterator;
	for(rowIterator = nodesList.begin(),indexRow=0;rowIterator!=nodesList.end();++rowIterator,++indexRow){
		string rowNode = *rowIterator;
		costMatrix[indexRow] = (int*)malloc(sizeof(int)*numberOfNodes);
		for(colIterator = nodesList.begin(),indexColumn=0;colIterator!=nodesList.end();++colIterator,++indexColumn){
			string colNode = *colIterator;
			costMatrix[indexRow][indexColumn] =infinity;
		}
	}
	costMatrix[findIndex(hostName)][findIndex(hostName)]=0;
	list<IP_Address>:: iterator iterator;
	indexRow = findIndex(hostName);
	for(iterator = neighbors.begin();iterator!=neighbors.end();++iterator){
			string node = *iterator;
			indexColumn = findIndex(node);
			costMatrix[indexRow][indexColumn] = 1;
	}
	constructRoutingTable();
	updateRoutingTable(0);
}


void printGraph(){
	int indexRow,indexColumn;	
	list<IP_Address>:: iterator colIterator,rowIterator;
	list<IP_Address>:: iterator it;
	cout<<"\t";
	for(it = nodesList.begin();it!=nodesList.end();++it){
		string node = *it;
		cout<<node.substr(0,2)<<"\t";
	}
	cout << "\n";
	for(rowIterator = nodesList.begin(),indexRow=0;rowIterator!=nodesList.end();++rowIterator,++indexRow){
		string rowNode = *rowIterator;
		cout << rowNode.substr(0,2)<<"\t";
		for(colIterator = nodesList.begin(),indexColumn=0;colIterator!=nodesList.end();++colIterator,++indexColumn){
			string colNode = *colIterator;
			cout<<costMatrix[indexRow][indexColumn]<<"\t";
		}
		cout<<"\n";
	}
}

int findIndex(IP_Address host){
	int index = -1;
	list<IP_Address>:: iterator iterator;
	for(iterator = nodesList.begin(),index=0;iterator!=nodesList.end();++iterator,++index){
		string node = *iterator;
		if(host.compare(node)==0){
			return index;
		}
	}
	return index;
}

void lostNode(IP_Address host){
	int rowIndex = findIndex(host);
	costMatrix[findIndex(hostName)][rowIndex] = infinity;
	RouteEntry entry = RoutingTable[host];
	entry.TTL = ttl;
	entry.cost = infinity;
	entry.nextHop = "NULL";
	RoutingTable[host] = entry;
	list<IP_Address>:: iterator it1;
	for(it1 = nodesList.begin();it1!=nodesList.end();++it1){
			string node = *it1;
			costMatrix[rowIndex][findIndex(node)] = infinity;
	}
}

int updateNode(AdversisementEntry tempNode,IP_Address host){
	int rowIndex = findIndex(host);
	int colIndex = findIndex(tempNode.ipAddr);
	int oldCost;
	costMatrix[findIndex(hostName)][rowIndex] =1;
	//cout <<rowIndex<<" "<<colIndex <<" "<<tempNode.cost<<endl;	
	costMatrix[rowIndex][colIndex] = tempNode.cost;
	RouteEntry tempEntry = RoutingTable[tempNode.ipAddr];
	oldCost = tempEntry.cost;
	if(tempEntry.nextHop.compare(host)==0){
		if(tempNode.cost+1!=tempEntry.cost){
			tempEntry.cost = infinity;
			tempEntry.nextHop = "NULL";
			RoutingTable[tempNode.ipAddr] = tempEntry;
			if(oldCost != infinity)
				return 1;
		}
	}
	return 0;
}
