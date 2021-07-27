#include "IPTools.h"

ULONG UDP_Network::createIP(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
	return (a << 24) | (b << 16) | (c << 8) | d;
}

ULONG UDP_Network::createIP(char* ip) {
	uint8_t a = ip[3];
	uint8_t b = ip[2];
	uint8_t c = ip[1];
	uint8_t d = ip[0];
	return (a << 24) | (b << 16) | (c << 8) | d;
}

void UDP_Network::getIP(ULONG ip, char* ipTo) {
	ipTo[3] = ip & 0xff;
	ip >>= 8;
	ipTo[2] = ip & 0xff;
	ip >>= 8;
	ipTo[1] = ip & 0xff;
	ip >>= 8;
	ipTo[0] = ip & 0xff;
}

bool UDP_Network::isAnyIP(char* ip) {
	return ip[3] == 0 && ip[2] == 0 && ip[1] == 0 && ip[0] == 0;
}

bool UDP_Network::equalIP(char* ip1, char* ip2) {
	return ip1[0] == ip2[0] && ip1[1] == ip2[1] && ip1[2] == ip2[2] && ip1[3] == ip2[3];
}