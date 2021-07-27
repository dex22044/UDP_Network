#pragma once

#include <winsock.h>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <queue>
#include <list>

namespace UDP_Network {
	ULONG createIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
	ULONG createIP(char* ip);
	void getIP(ULONG ip, char* ipTo);
	bool isAnyIP(char* ip);
	bool equalIP(char* ip1, char* ip2);
}