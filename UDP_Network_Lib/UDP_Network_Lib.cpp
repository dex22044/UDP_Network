#include "UDP_Network_Lib.h"

bool UDP_Network::UDP_Network_Connector::Connect(ULONG serverAddr, int serverPort)
{
	client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	{
		sockaddr_in sa;
		sa.sin_family = AF_INET;
		sa.sin_port = htons(8567);
		sa.sin_addr.S_un.S_addr = INADDR_ANY;
		assert(bind(client, (sockaddr*)&sa, sizeof(sockaddr_in)) == 0);
	}

	serverSA.sin_family = AF_INET;
	serverSA.sin_port = htons(serverPort);
	serverSA.sin_addr.S_un.S_addr = serverAddr;
			
	u_long mode = 1;
	ioctlsocket(client, FIONBIO, &mode);

	isRXLocked = false;
	for (int i = 0; i < 8; i++) {
		if (loggingEnabled) printf("Getting public IP (attempt %d)...", i + 1);
		RequestIP();
		printf("requested...");
		Sleep(500);
		Poll();
		Sleep(250);
		if (!RX_Buffer.empty()) {
			pair<int, Message*> msg = RX_Buffer.front();
			RX_Buffer.pop();
			if (loggingEnabled) printf("Got public IP: ");
			if (loggingEnabled) printf("%d.%d.%d.%d:%d\n",
				(unsigned char) msg.second->getData()[0],
				(unsigned char) msg.second->getData()[1],
				(unsigned char) msg.second->getData()[2],
				(unsigned char) msg.second->getData()[3],
				((uint16_t*)(msg.second->getData() + 4))[0]);
			publicSA.sin_family = AF_INET;
			publicSA.sin_addr.S_un.S_addr = htonl(createIP(msg.second->getData()));
			publicSA.sin_port = htons(((uint16_t*)(msg.second->getData() + 4))[0]);
			free(msg.second);
			return true;
		}
		if (loggingEnabled) printf("failed.\n");
		Sleep(500);
	}
	return false;
}

void UDP_Network::UDP_Network_Connector::RequestIP() {
	Message* msg = (Message*)calloc(1, 65500);
	msg->msgIdentityKey = rand();
	msg->isToServer = true;
	msg->getData()[0] = 1;
	sendto(client, (char*)msg, sizeof(Message) + 1, 0, (sockaddr*)&serverSA, sizeof(sockaddr_in));
	free(msg);
}

void UDP_Network::UDP_Network_Connector::Poll() {
	while (isRXLocked) continue;
	isRXLocked = true;
	while (true) {
		if (loggingEnabled) printf("polling messages...");
		Message* msg = (Message*)calloc(1, 65500);
		int msgLen = recv(client, (char*)msg, 65500, 0);
		if (msgLen <= 0) {
			int err = WSAGetLastError();
			if(err != WSAEWOULDBLOCK && loggingEnabled) printf("Error: %d.\n", err);
			free(msg);
			isRXLocked = false;
			return;
		}

		RX_Buffer.push(make_pair(msgLen, msg));
	}
	isRXLocked = false;
}

void UDP_Network::UDP_Network_Connector::ProcessMessages()
{
	while (isRXLocked) continue;
	isRXLocked = true;
	while (!RX_Buffer.empty()) {
		pair<int, Message*> msg = RX_Buffer.front();
		RX_Buffer.pop();
		char* data = msg.second->getData();

		if (msg.second->isToServer) {
			free(msg.second);
			continue;
		}

		if (data[0] == 4) {
			int localClientId = ((int*)(data + 1))[0];
			int remoteClientId = ((int*)(data + 5))[0];
			for (Client* c : clients) {
				if (c->localClientId == localClientId) {
					c->EnqueueMessage(msg.first, msg.second);
				}
			}
		}
	}
	isRXLocked = false;
}