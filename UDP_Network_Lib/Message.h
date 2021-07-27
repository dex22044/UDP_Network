#pragma once

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