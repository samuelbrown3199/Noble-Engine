#include <enet/enet.h>

#include "PacketParser.h"

class NetworkManager
{
public:

	PacketParser* m_parser;

	NetworkManager(const int& _type);
	~NetworkManager();
};