#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;
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

	rv = getaddrinfo("localhost", "22373", &hints, &res);
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

	freeaddrinfo(res);

	printf("The Server B is up and running using UDP on port 22373\n");
	while (1) {
		addr_size = sizeof their_addr;
	recv_res = recvfrom(sock, buf, 99, 0, (struct sockaddr *)&their_addr, &addr_size);
	if (recv_res == -1)
	{
		perror("recvfrom");
		exit(1);
	}
	vector<string> AWSInput;
	string temp = "";
	for (int i = 0; i < strlen(buf); ++i)
	{
		if (buf[i] == '/')
		{
			AWSInput.push_back(temp);
			temp = "";

		}
		else {
			temp = temp + buf[i];
		}
	}

	printf("The Server B has received data for calculation: \n");
	cout << "* Propagation speed: " << AWSInput.at(AWSInput.size()-3) << " km/s;"  << endl;
	cout << "* Transmission speed: " << AWSInput.at(AWSInput.size()-2) << " Bytes/s;"  << endl;
	for (int i = 0; i < (AWSInput.size()/2-1); ++i)
	{
		int temp = i*2;
		cout << "* Path length for destination " << AWSInput.at(temp) << ": " << AWSInput.at(temp+1) << ";" << endl;
	}

	printf("The Server B has finished the calculation of the delays: \n");
	cout << "-----------------------------------------" << endl;
	cout << setiosflags(ios::left) << setw(20) << "Destination" << setw(30) << "Delay" << setiosflags(ios::left) << endl;
	cout << "-----------------------------------------" << endl;
	double Tt = atof(AWSInput.at(AWSInput.size()-1).c_str()) / atof(AWSInput.at(AWSInput.size()-2).c_str()) / 8;
	vector<string> sentToAWS;
	for (int i = 0; i < (AWSInput.size()/2-1); ++i)
	{
		int temp = i*2;
		double Tp = atof(AWSInput.at(temp+1).c_str()) / atof(AWSInput.at(AWSInput.size()-3).c_str());
		sentToAWS.push_back(AWSInput.at(temp));
		sentToAWS.push_back(to_string(Tp));
		sentToAWS.push_back(to_string(Tp+Tt));
		cout << setiosflags(ios::left) << setw(20) << AWSInput.at(temp) << setw(30) << setprecision(2) << fixed << Tp+Tt << setiosflags(ios::left) << endl;
	}
	sentToAWS.push_back(to_string(Tt));
	cout << "-----------------------------------------" << endl;

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
	
	string sendstring;
	for (int i = 0; i < sentToAWS.size(); ++i)
	{
		sendstring = sendstring + sentToAWS.at(i) + "/";
	}
	send_res = sendto(sock_send, sendstring.c_str(), 300, 0, res_send->ai_addr, res_send->ai_addrlen);
	if (send_res == -1)
	{
		perror("sendto");
		exit(1);
	}
	freeaddrinfo(res_send);
	
	printf("The Server B has finished sending the output to AWS.\n");


	}
	
	return 0;
}
