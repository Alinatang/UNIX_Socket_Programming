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
	struct addrinfo hints_server, *res_server;
	struct addrinfo *servinfo_server;
	int sock_server;
	struct sockaddr_in sin_server, peer_addr;
	int len_server = sizeof(sin_server);
	int rv_server;
	int send_res_server;
	int send_res;

	struct addrinfo hints_client, *res_client, *p;
	struct addrinfo *servinfo_client;
	int sock_client;
	struct sockaddr_in sin_client;
	int len_client = sizeof(sin_client);
	int rv_client;
	int recv_res_client;
	char buf_client[100];

	struct sockaddr_storage their_addr_client;
	socklen_t addr_size_client;
	int new_fd_client;

	memset(&hints_server, 0, sizeof hints_server);
	hints_server.ai_family = AF_UNSPEC;
	hints_server.ai_socktype = SOCK_DGRAM;
	hints_server.ai_protocol = 0;

	memset(&hints_client, 0, sizeof hints_client);
	hints_client.ai_family = AF_UNSPEC;
	hints_client.ai_socktype = SOCK_STREAM;
	hints_client.ai_protocol = 0;

	

	// getaddrinfo("localhost", "23373", &hints_server, &res_server);
	// sock_server = socket(res_server->ai_family, res_server->ai_socktype, res_server->ai_protocol);
	// bind(sock_server, res_server->ai_addr, res_server->ai_addrlen);

	rv_client = getaddrinfo("localhost", "24373", &hints_client, &res_client);
	if (rv_client != 0)
	{
		fprintf(stderr, "getaddringo: %s\n", gai_strerror(rv_client));
		return 1;
	}
	for (p = res_client; p != NULL; p = p->ai_next){
		sock_client = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock_client == -1)
		{
			perror("server: socket");
			continue;
		}
		if (bind(sock_client, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sock_client);
			perror("server: bind");
			continue;
		}
		break;
	}

	rv_server = getaddrinfo("localhost", "23373", &hints_server, &res_server);
	if (rv_server != 0)
	{
		fprintf(stderr, "getaddringo: %s\n", gai_strerror(rv_server));
		return 1;
	}
	for (p = res_server; p != NULL; p = p->ai_next){
		sock_server = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock_server == -1)
		{
			perror("server: socket");
			continue;
		}
		if (bind(sock_server, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(sock_server);
			perror("server: bind");
			continue;
		}
		break;
	}

	freeaddrinfo(res_client);
	freeaddrinfo(res_server);

	printf("AWS is up and running.\n");

	if (listen(sock_client, 10) == -1)
	{
		perror("listen");
		exit(1);
	}
	;

	while(1){
		addr_size_client = sizeof their_addr_client;
		new_fd_client = accept(sock_client, (struct sockaddr *)&their_addr_client, &addr_size_client);
		if (new_fd_client == -1)
		{
			perror("accept");
			continue;
		}
		
		recv_res_client = recv(new_fd_client, buf_client, 99, 0);
		if(recv_res_client == -1){
			perror("recv");
			exit(1);
		}
		
		string clientInput[3];
		int index = 0;
		for (int i = 0; i < strlen(buf_client); ++i)
		{
			if (buf_client[i] == '/')
			{
				index++;
			}
			else {
				clientInput[index] = clientInput[index] + buf_client[i];
			}
		}
		cout << "The AWS has received map ID " << clientInput[0] << ", start vertex " << clientInput[1] << " and file size " << clientInput[2] << " from the client using TCP over port 24373" << endl;

		//sent to serverA
		struct addrinfo hints_send, *res_send, *p;
		int sock_send;
		int send_res;
		int rv_send;


		memset(&hints_send, 0, sizeof hints_send);
		hints_send.ai_family = AF_UNSPEC;
		hints_send.ai_socktype = SOCK_DGRAM;
		hints_send.ai_protocol = 0;
		
		rv_send = getaddrinfo("localhost", "21373", &hints_send, &res_send);
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
		string sentToA = clientInput[0] + "." + clientInput[1];
		send_res = sendto(sock_send, sentToA.c_str(), 20, 0, res_send->ai_addr, res_send->ai_addrlen);
		if (send_res == -1)
		{
			perror("sendto");
			exit(1);
		}
		freeaddrinfo(res_send);
		printf("The AWS has sent map ID and starting vertex to server A using UDP over port 23373\n");

		//get info back from serverA
        struct sockaddr_storage their_addr_serverA;
		socklen_t addr_size_serverA;
		int recv_res_serverA;
		char buf_serverA[200];

		addr_size_serverA = sizeof their_addr_serverA;
		recv_res_serverA = recvfrom(sock_server, buf_serverA, 199, 0, (struct sockaddr *)&their_addr_serverA, &addr_size_serverA);
		if (recv_res_serverA == -1)
		{
			perror("recvfrom");
			exit(1);
		}
		vector<string> serverAInput;
		string temp = "";
		for (int i = 0; i < strlen(buf_serverA); ++i)
		{
			if (buf_serverA[i] == '/')
			{
				serverAInput.push_back(temp);
				temp = "";

			}
			else {
				temp = temp + buf_serverA[i];
			}
		}
		printf("The AWS has received shortest path from server A: \n");
		cout << "-----------------------------------------" << endl;
		cout << setiosflags(ios::left) << setw(20) << "Destination" << setw(30) << "Min Length" << setiosflags(ios::left) << endl;
		cout << "-----------------------------------------" << endl;
		for (int i = 0; i < (serverAInput.size()/2 - 1); ++i)
		{
			int temp = i*2;
			cout << setiosflags(ios::left) << setw(20) << serverAInput.at(temp) << setw(30) << serverAInput.at(temp+1) << setiosflags(ios::left) << endl;
		}
		cout << "-----------------------------------------" << endl;

		//send to serverB

		struct addrinfo hints_send_B, *res_send_B;
		int sock_send_B;
		int send_res_B;
		int rv_send_B;


		memset(&hints_send_B, 0, sizeof hints_send_B);
		hints_send_B.ai_family = AF_UNSPEC;
		hints_send_B.ai_socktype = SOCK_DGRAM;
		hints_send_B.ai_protocol = 0;
		
		rv_send_B = getaddrinfo("localhost", "22373", &hints_send_B, &res_send_B);
		if (rv_send_B != 0)
		{
			fprintf(stderr, "getaddringo: %s\n", gai_strerror(rv_send_B));
			return 1;
		}

		sock_send_B = socket(res_send_B->ai_family, res_send_B->ai_socktype, res_send_B->ai_protocol);
		if (sock_send_B == -1)
		{
			perror("aws: send socket"); 

		}
		
		string sentToB = buf_serverA; 
		sentToB = sentToB + clientInput[2] + '/';
		send_res_B = sendto(sock_send_B, sentToB.c_str(), 200, 0, res_send_B->ai_addr, res_send_B->ai_addrlen);
		if (send_res_B == -1)
		{
			perror("sendto");
			exit(1);
		}
		freeaddrinfo(res_send_B);
		printf("The AWS has sent path length, propagation speed and transmission speed to server B using UDP over port 23373\n");

		//get info back from serverB
        struct sockaddr_storage their_addr_serverB;
		socklen_t addr_size_serverB;
		int recv_res_serverB;
		char buf_serverB[300];

		addr_size_serverB = sizeof their_addr_serverB;
		recv_res_serverB = recvfrom(sock_server, buf_serverB, 299, 0, (struct sockaddr *)&their_addr_serverB, &addr_size_serverB);
		if (recv_res_serverB == -1)
		{
			perror("recvfrom");
			exit(1);
		}

		vector<string> serverBInput;
		string temp2 = "";
		for (int i = 0; i < strlen(buf_serverB); ++i)
		{
			if (buf_serverB[i] == '/')
			{
				serverBInput.push_back(temp2);
				temp2 = "";
			}
			else {
				temp2 = temp2 + buf_serverB[i];
			}
		}		
		printf("The AWS has received delays from server B:\n");
		cout << "---------------------------------------------------" << endl;
		cout << setiosflags(ios::left) << setw(20) << "Destination" << setw(10) << "Tt" << setw(10) << "Tp" << setw(10) << "Delay" << setiosflags(ios::left) << endl;
		cout << "---------------------------------------------------" << endl;

		for (int i = 0; i < serverBInput.size()/3; ++i)
		{
			int temp = i*3;
			cout << setiosflags(ios::left) << setw(20) << serverBInput.at(temp) << setw(10) << setprecision(2) << fixed << atof(serverBInput.at(serverBInput.size()-1).c_str()) << setw(10) << setprecision(2) << fixed << atof(serverBInput.at(temp+1).c_str()) << setw(10) << setprecision(2) << fixed << atof(serverBInput.at(temp+2).c_str()) << setiosflags(ios::left) << endl;
		}
		cout << "---------------------------------------------------" << endl;

		//send to client
		
		string sendToCLient;
		int index2 = 1;
		for (int i = 0; i < serverBInput.size()/3; ++i)
		{
			int temp = i*3;
			sendToCLient = sendToCLient + serverBInput.at(temp) + "/"  + serverAInput.at(index2) + "/" + serverBInput.at(serverBInput.size()-1) + "/" + serverBInput.at(temp+1) + "/" + serverBInput.at(temp+2) + "/";
			index2 = index2 + 2;
		}
		send_res = send(new_fd_client, sendToCLient.c_str(), 500, 0);
	    if (send_res == -1)
		{
			perror("send"); 
			exit(1);
		}
		else if (send_res != -1)
		{
			printf("The AWS has sent calculated delay to client using TCP over port 24373\n");
		}
	
	}
	// printf("%d\n", ntohs(peer_addr.sin_port));
	return 0;
}
