#include <winsock.h>
#include <iostream>
#include <cassert>
#include <cstdint>

class Message
{
public:
	bool isToServer;
	int msgIdentityKey;
	char ipFrom[4];
	uint16_t portFrom;
	char ipFromServSet[4];
	uint16_t portFromServSet;
	char ipTo[4];
	uint16_t portTo;

	char* getData() {
		return ((char*)this) + sizeof(Message);
	}
};

using namespace std;

#pragma comment(lib, "ws2_32.lib")

ULONG createIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
	return (a << 24) | (b << 16) | (c << 8) | d;
}

ULONG createIP(char* ip) {
	uint8_t a = ip[0];
	uint8_t b = ip[1];
	uint8_t c = ip[2];
	uint8_t d = ip[3];
	return (a << 24) | (b << 16) | (c << 8) | d;
}

void getIP(ULONG ip, char* ipTo) {
	ipTo[3] = ip & 0xff;
	ip >>= 8;
	ipTo[2] = ip & 0xff;
	ip >>= 8;
	ipTo[1] = ip & 0xff;
	ip >>= 8;
	ipTo[0] = ip & 0xff;
}

bool isAnyIP(char* ip) {
	return ip[3] == 0 && ip[2] == 0 && ip[1] == 0 && ip[0] == 0;
}

bool equalIP(char* ip1, char* ip2) {
	return ip1[0] == ip2[0] && ip1[1] == ip2[1] && ip1[2] == ip2[2] && ip1[3] == ip2[3];
}

int main()
{
	{
		WSAData d;
		assert(WSAStartup(MAKEWORD(2, 2), &d) == 0);
	}

	SOCKET server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	{
		sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(9000);
		sa.sin_addr.S_un.S_addr = INADDR_ANY;
		assert(bind(server, (sockaddr*)&sa, sizeof(sockaddr_in)) == 0);
	}

	sockaddr_in sa_remote;
	ZeroMemory(&sa_remote, sizeof(sockaddr_in));
	int sa_remote_len = sizeof(sockaddr_in);
	Message* msg = (Message*)calloc(1, 65500);
	while (true) {
		Sleep(500);

		int pktLen = recvfrom(server, (char*)msg, 65500, 0, (sockaddr*)&sa_remote, &sa_remote_len);

		if (pktLen <= 0) {
			printf("Error: %d\n", WSAGetLastError());
			continue;
		}

		printf("request (%d)\n", pktLen);

		if (msg->isToServer) {
			char* msgData = msg->getData();
			if (msgData[0] == 1) {
				// Get IP
				getIP(ntohl(sa_remote.sin_addr.S_un.S_addr), msgData);
				int p = ntohs(sa_remote.sin_port);
				memcpy(msgData + 4, &p, 2);
				sendto(server, (char*)msg, sizeof(Message) + 6, 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in));
			}
			if (msgData[0] == 2) {
				// Connection check
				sendto(server, (char*)msg, pktLen, 0, (sockaddr*)&sa_remote, sizeof(sockaddr_in));
			}
		}
		else {
			if (!isAnyIP(msg->ipFromServSet)) { printf("IP invalid 1\n"); continue; }
			if (msg->portFromServSet != 0) { printf("IP invalid 2\n"); continue; }

			if (isAnyIP(msg->ipFrom)) { printf("IP invalid 3\n"); continue; }
			if (msg->portFrom == 0) { printf("IP invalid 4\n"); continue; }

			getIP(sa_remote.sin_addr.S_un.S_addr, msg->ipFromServSet);
			msg->portFromServSet = ntohs(sa_remote.sin_port);

			if (!equalIP(msg->ipFrom, msg->ipFromServSet)) { printf("IP invalid 5\n"); continue; }
			if (msg->portFrom != msg->portFromServSet) { printf("IP invalid 6\n"); continue; }

			sockaddr_in sa;
			sa.sin_family = AF_INET;
			sa.sin_port = htons(msg->portTo);
			sa.sin_addr.S_un.S_addr = htonl(createIP(msg->ipTo));
			sendto(server, (char*)msg, pktLen, 0, (sockaddr*)&sa, sizeof(sockaddr_in));
		}
	}
}