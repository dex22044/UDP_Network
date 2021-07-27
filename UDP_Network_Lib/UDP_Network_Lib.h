#pragma once

#include <winsock.h>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <queue>
#include <list>

#include "Client.h"
#include "IPTools.h"
#include "Message.h"

using namespace std;

#pragma comment(lib, "ws2_32.lib")

namespace UDP_Network {
	class UDP_Network_Connector {
	private:
		queue<pair<int, Message*>> RX_Buffer;
		bool isRXLocked = false;

	public:
		SOCKET client;
		sockaddr_in serverSA;
		sockaddr_in publicSA;

		list<Client*> clients;

		bool loggingEnabled = true;

		bool Connect(ULONG serverAddr, int serverPort);
		void RequestIP();
		void Poll();
		void ProcessMessages();
	};
}