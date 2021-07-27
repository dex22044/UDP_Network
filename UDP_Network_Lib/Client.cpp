#include "Client.h"

void UDP_Network::Client::EnqueueMessage(int len, Message* msg)
{
	RX_Buffer.push(make_pair(len, msg));
}

void UDP_Network::Client::Send(char* buf, int len)
{
	Message* msg = (Message*)calloc(1, len + sizeof(Message) + 9);
	msg->isToServer = false;
	getIP(ntohl(publicSA.sin_addr.S_un.S_addr), msg->ipFrom);
	msg->portFrom = ntohs(publicSA.sin_port);
	getIP(remoteAddr, msg->ipTo);
	msg->portTo = remotePort;
	msg->msgIdentityKey = rand();
	char* data = msg->getData();
	data[0] = 4;
	memcpy(data + 1, &remoteClientId, 4);
	memcpy(data + 5, &localClientId, 4);
	memcpy(data + 9, buf, len);
	sendto(sock, (char*)msg, len + sizeof(Message) + 9, 0, (sockaddr*)&serverSA, sizeof(sockaddr_in));
	free(msg);
}

int UDP_Network::Client::Receive(char* buf)
{
	while (!RX_Buffer.empty()) {
		pair<int, Message*> msg = RX_Buffer.front();
		RX_Buffer.pop();
		char* data = msg.second->getData();
		if (data[0] == 4) {
			memcpy(buf, data + 9, msg.first - sizeof(Message) - 9);
			return msg.first - sizeof(Message) - 9;
		}
		free(msg.second);
	}
	return 0;
}