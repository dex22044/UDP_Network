#include <winsock.h>
#include <iostream>
#include <cassert>
#include <cstdint>

#include <UDP_Network_Lib.h>

using namespace std;
using namespace UDP_Network;

#pragma comment(lib, "ws2_32.lib")

int main()
{
	WSAData d;
	assert(WSAStartup(MAKEWORD(2, 2), &d) == 0);

	UDP_Network_Connector* conn = new UDP_Network_Connector();

	if(!conn->Connect(htonl(createIP(192, 168, 1, 2)), 9000)) exit(-1);

	string ip;
	cout << "Remote IP: ";
	cin >> ip;

	Client* client1 = new Client(conn->client, conn->serverSA, conn->publicSA, ntohl(inet_addr(ip.c_str())), 8567);

	conn->clients.push_back(client1);

	client1->localClientId = rand();
	printf("Local client ID: %d\n", client1->localClientId);
	cout << "Remote client ID: ";
	cin >> client1->remoteClientId;

	char buf1[256];
	char buf2[256];

	memcpy(buf1, "It works? It works.", 20);

	while (true) {
		//client1->Send(buf1, 256);
		Sleep(250);
		conn->Poll();
		conn->ProcessMessages();
		int len = client1->Receive(buf2);
		printf("%d: %s\n", len, buf2);
		Sleep(500);
	}

	system("pause");
}