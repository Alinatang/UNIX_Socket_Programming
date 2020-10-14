1. I realize the whole process of transmission in this project. The client can get the delays for each destination according to the map and start vertex that client chooses. ServerA, serverB and AWS will keep working until the TA stops it manually. Client can request results several times when servers keep working.

2. client.cpp: 
	In this file, the client creates a socket to connect with AWS. And then it sends arguments inputted in the command line to AWS. Finally, client receives the delays for each vertex, and prints them out.
   aws.cpp:
	In this file, AWS communicates with client through port 24373 and communicates with two servers through port 23373. AWS gets the requirement from client and sends it to serverA. After getting results calculated by serverA, AWS sends them to serverB and get value of delays back. Finally, AWS sends delays to client and wait for new connection.
   serverA.cpp:
	In this file, serverA communicate with AWS through port 21373. At the beginning, it reads map data file anf store the data in different vectors. After getting requirement from AWS, it calculates the shortest path from given start vertex to all other vertices in the same map, and then sends the results to AWS.
   serverB.cpp:
	In this file, serverB communicate with AWS through port 22373. It gets all shortest paths and speed of transmission and propagation from AWS. Then it calculates all delays and sends to AWS.

3. The format of data that transmit through the network is string. All the results are combined as a single string with ¡°/¡° inserted between each result. After receiving the string, all devices get useful information by dividing the string into pieces according to the location of ¡°/¡°.

4. Through my tests, all files work will and can get right results as expected.

5. I use code from "Beej's Guide to Network Programming", the methods are:
	int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
	int socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
	int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
	int listen(int sockfd, int backlog);
	int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	int send(int sockfd, const void *msg, int len, int flags);
	int recv(int sockfd, void *buf, int len, int flags);
	int sendto(int sockfd, const void *msg, int len, unsigned int flags, const struct sockaddr *to, socklen_t tolen);
	int recvfrom(int sockfd, void *buf, int len, unsigned int flags, struct sockaddr *from, int *fromlen);
	close(sockfd);

   I use code from "https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/" for Dijkstra's Algorithm.
	class Graph;
	Graph::Graph(int V);
	void Graph::addEdge(int u, int v, int w);
	void Graph::shortestPath(int src);
	g.addEdge(a, b, c);
