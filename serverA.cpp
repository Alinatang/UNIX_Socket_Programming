#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <bits/stdc++.h>
#include <iterator>

using namespace std;

#define INF 0x3f3f3f3f
typedef pair<int, int> iPair;
class Graph {
	int V;
	list<pair<int, int> > *adj;
public:
	Graph(int V);
	void addEdge(int u, int v, int w);
	string shortestPath(int s, int m);
};
Graph::Graph(int V) {
	this->V = V;
	adj = new list<iPair> [V];
}
void Graph::addEdge(int u, int v, int w) {
	adj[u].push_back(make_pair(v,w));
	adj[v].push_back(make_pair(u,w));
}
string Graph::shortestPath(int src, int minus) {
	priority_queue< iPair, vector <iPair>, greater<iPair> > pq;
	vector<int> dist(V, INF);
	pq.push(make_pair(0,src));
	dist[src] = 0;
	while (!pq.empty()) {
		int u = pq.top().second;
		pq.pop();
		list< pair<int, int> >::iterator i;
		for (i = adj[u].begin(); i != adj[u].end(); ++i)
		{
			int v= (*i).first;
			int weight = (*i).second;
			if (dist[v] > dist[u] + weight)
			{
				dist[v] = dist[u] + weight;
				pq.push(make_pair(dist[v], v));
			}
		}
	}
	string result;
	cout << "The Server A has identified the following shortest paths: " << endl;
	cout << "-----------------------------------------" << endl;
	printf("Destination    Min Length\n");
	cout << "-----------------------------------------" << endl;
	for (int i = 0; i < V; ++i)
	{
		if(i != src) {
			printf("%d \t\t %d\n", i + minus, dist[i]);
			result = result + to_string(i + minus) + "/" + to_string(dist[i]) + "/";
		}
	}
	cout << "-----------------------------------------" << endl;
	return result;
}

int main(int argc, char const *argv[])
{

	struct addrinfo hints, *res, *p;
	int sock;
	struct sockaddr_in sin;
	int len = sizeof(sin);
	int rv;
	int recv_res;
	char buf[100];

	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	int new_fd;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0;

	struct addrinfo hints_send, *res_send;
	int sock_send;
	int send_res;
	int rv_send;

	memset(&hints_send, 0, sizeof hints_send);
	hints_send.ai_family = AF_UNSPEC;
	hints_send.ai_socktype = SOCK_DGRAM;
	hints_send.ai_protocol = 0;

	rv = getaddrinfo("localhost", "21373", &hints, &res);
	if (rv != 0)
	{
		fprintf(stderr, "getaddringo: %s\n", gai_strerror(rv));
		return 1;
	}
	for (p = res; p != NULL; p = p->ai_next){
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock == -1)
		{
			perror("server: socket");
			continue;
		}
		if (bind(sock, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sock);
			perror("server: bind");
			continue;
		}
		break;
	}
	printf("Server A is up and running using UDP on port 21373.\n");

	vector<int> mapinfo;
	vector<vector<int> > map;
	string mapname_temp;
	vector<string> mapname;
	vector<double> speed_prop;
	vector<double> speed_tran;
	double speed_prop_temp;
	double speed_tran_temp;
	ifstream infile("testmap.txt");
	string test;
	
	if (!infile.is_open()) {
		cout << "cannot open the file" <<endl;
		return 0;
	}
	infile >> mapname_temp;
	mapname.push_back(mapname_temp);
	while(! infile.eof()) {
		infile >> speed_prop_temp;
		speed_prop.push_back(speed_prop_temp);
		infile >> speed_tran_temp;
		speed_tran.push_back(speed_tran_temp);
		mapinfo.clear();
		while(! infile.eof()) {
			infile >> test;
			if (isdigit(test.at(0)))
			{
				int a = atoi(test.c_str());
				mapinfo.push_back(a);
			}
			else {
				break;
			}
		}
		map.push_back(mapinfo);
		if (! infile.eof())
		{
			mapname.push_back(test);
		}	
	}
	map.at(map.size()-1).pop_back();
	vector<int> edgenum;
	vector<vector<int> > vertices;
	for (int i = 0; i < map.size(); ++i)
	{
		mapinfo = map.at(i);
		int edgenum_temp = mapinfo.size() / 3;
		edgenum.push_back(edgenum_temp);
		vector<int> vertices_temp;
		vertices_temp.push_back(mapinfo.at(0));
		for (int i = 1; i < mapinfo.size(); i++)
		{
			if ((i+1) % 3 != 0)
			{
				int check = 0;
				for (int j = 0; j < vertices_temp.size(); j++)
				{
					if (mapinfo.at(i) == vertices_temp.at(j))
					{	
						check = 1;
					}
				}
				if (check == 0)
				{
					vertices_temp.push_back(mapinfo.at(i));
				}
			}
		}
		vertices.push_back(vertices_temp);
	}
	
	cout << "The Server A has constructed a list of " << map.size() << " maps:" << endl;
	cout << "-----------------------------------------" << endl;
	cout << setiosflags(ios::left) << setw(8) << "Map ID" << setw(15) << "NUm Vertices" << setw(15) << "Num Edges" << setiosflags(ios::left) << endl;
	cout << "-----------------------------------------" << endl;
	for (int i = 0; i < map.size(); ++i)
	{
		cout << setiosflags(ios::left) << setw(8) << mapname.at(i) << setw(15) << vertices.at(i).size() << setw(15) << edgenum.at(i) << setiosflags(ios::left) << endl;
	}
	cout << "-----------------------------------------" << endl;

	freeaddrinfo(res);
	
	while (1) {

	addr_size = sizeof their_addr;
	recv_res = recvfrom(sock, buf, 99, 0, (struct sockaddr *)&their_addr, &addr_size);
	if (recv_res == -1)
	{
		perror("recvfrom");
		exit(1);
	}

	buf[recv_res] = '\0';

	string awsInput[2];
	int index_temp = 0;
	for (int i = 0; i < strlen(buf); ++i)
	{
		if (buf[i] == '.')
		{
			index_temp++;
		}
		else {
			awsInput[index_temp] = awsInput[index_temp] + buf[i];
		}
	}
	cout << "The Server A has received input for finding shortest paths: starting vertex " << awsInput[1] << " of map " << awsInput[0] << "." << endl;
	int index;
	for (int i = 0; i < mapname.size(); ++i)
	{
		if(awsInput[0] == mapname.at(i)) {
			index = i;
		}
	}
	vector<int> usedmap = map.at(index);
	int minus = usedmap.at(0);
	for (int i = 0; i < usedmap.size()/3; i++)
	{
		int temp = i*3;
		if (minus > usedmap.at(temp))
		{
			minus = usedmap.at(temp);
		}
		if (minus > usedmap.at(temp+1))
		{
			minus = usedmap.at(temp+1);
		}
	}
	int V = vertices.at(index).size();
	Graph g(V);
	for (int i = 0; i < usedmap.size()/3; i++)
	{
		int temp = i*3;
	 	g.addEdge(usedmap.at(temp) - minus,usedmap.at(temp+1) - minus,usedmap.at(temp+2));
	 }
	
	string result = g.shortestPath(atoi(awsInput[1].c_str()) - minus, minus);
	result = result + to_string(speed_prop.at(index)) + "/" + to_string(speed_tran.at(index)) + "/";
	
	//send back to aws

	rv_send = getaddrinfo("localhost", "23373", &hints_send, &res_send);
	if (rv_send != 0)
	{
		fprintf(stderr, "getaddringo: %s\n", gai_strerror(rv_send));
		return 1;
	}

	sock_send = socket(res_send->ai_family, res_send->ai_socktype, res_send->ai_protocol);
	if (sock_send == -1)
	{
		perror("aws: send socket"); 

	}
	
	send_res = sendto(sock_send, result.c_str(), 100, 0, res_send->ai_addr, res_send->ai_addrlen);
	if (send_res == -1)
	{
		perror("sendto");
		exit(1);
	}
	freeaddrinfo(res_send);
	
	printf("The Server A has sent shortest paths to AWS.\n");

	}

	return 0;
}
