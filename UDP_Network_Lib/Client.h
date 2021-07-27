#pragma once

#include <winsock.h>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <queue>
#include <list>

#include "IPTools.h"
#include "Message.h"

using namespace std;

namespace UDP_Network {
	class Client {
	private:
		queue<pair<int, Message*>> RX_Buffer;
		bool isRXLocked = false;

		sockaddr_in serverSA;
		sockaddr_in publicSA;
		SOCKET sock;

	public:
		ULONG remoteAddr;
		int remotePort;

		int localClientId;
		int remoteClientId;

		Client(SOCKET s, sockaddr_in serverSA, sockaddr_in publicSA, ULONG addr, int port)
			: sock(s), serverSA(serverSA), publicSA(publicSA), remoteAddr(addr), remotePort(port) {}

		void EnqueueMessage(int len, Message* msg);
		void Send(char* buf, int len);
		int Receive(char* buf);
	};
}