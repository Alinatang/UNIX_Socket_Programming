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

int main(int argc, char *argv[])
{

	printf("The client is up and running.\n");

	//connection begins.
	struct addrinfo hints_connect, *res_connect;
	int sock_connect;
	int connection_res;
	int rv;
	int send_res;
	int recv_res;
	char recvstring[500];

	memset(&hints_connect, 0, sizeof hints_connect);
	hints_connect.ai_family = AF_UNSPEC;
	hints_connect.ai_socktype = SOCK_STREAM;
	hints_connect.ai_protocol = 0;

	rv = getaddrinfo("localhost", "24373", &hints_connect, &res_connect);
	if (rv != 0)
	{
		fprintf(stderr, "getaddringo: %s\n", gai_strerror(rv));
		return 1;
	}

	sock_connect = socket(res_connect->ai_family, res_connect->ai_socktype, res_connect->ai_protocol);
	if (sock_connect == -1)
	{
		perror("client: socket"); 

	}
	connection_res = connect(sock_connect, res_connect->ai_addr, res_connect->ai_addrlen);
	if (connection_res == -1)
	{
		perror("client: connect");
	}

	freeaddrinfo(res_connect);

	string argvstring = (string)argv[1] + "/" + (string)argv[2] + "/" + (string)argv[3];

	send_res = send(sock_connect, argvstring.c_str(), 20, 0);

	if (send_res == -1)
	{
		perror("send"); 
		exit(1);
	}
	else if (send_res != -1)
	{
		cout << "The client has sent query to AWS using TCP: start vertex " << argv[2] << "; map " << argv[1] << "; file size " << argv[3] << "." << endl;
	}

	recv_res = recv(sock_connect, recvstring, 499, 0);
	if (recv_res == -1)
	{
		perror("receive");
		exit(1);
	}
	vector<string> AWSInput;
	string temp = "";
	for (int i = 0; i < strlen(recvstring); ++i)
	{
		if (recvstring[i] == '/')
		{
			AWSInput.push_back(temp);
			temp = "";
		}
		else {
			temp = temp + recvstring[i];
		}
	}
	printf("The client has received results from AWS: \n");
	cout << "--------------------------------------------------------------------" << endl;
	cout << setiosflags(ios::left) << setw(20) << "Destination" << setw(17) << "Min Length" << setw(10) << "Tt" << setw(10) << "Tp" << setw(10) << "Delay" << setiosflags(ios::left) << endl;
	cout << "--------------------------------------------------------------------" << endl;
	for (int i = 0; i < AWSInput.size()/5; ++i)
	{
		int temp = i*5;
		cout << setiosflags(ios::left) << setw(20) << AWSInput.at(temp) << setw(17) << AWSInput.at(temp+1) << setw(10) << setprecision(2) << fixed << atof(AWSInput.at(temp+2).c_str()) << setw(10) << setprecision(2) << fixed << atof(AWSInput.at(temp+3).c_str()) << setw(10) << setprecision(2) << fixed << atof(AWSInput.at(temp+4).c_str()) << setiosflags(ios::left) << endl;
	}
	cout << "--------------------------------------------------------------------" << endl;

	close(sock_connect);

	return 0;
}
 