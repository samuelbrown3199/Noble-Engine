#include <iostream>
#include <enet/enet.h>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <Windows.h>

#include <Engine/Networking/Packet.hpp>
#include <Engine/Networking/PacketParser.h>

std::mutex clientLock;

class Client;
void MessageServerLoop(Client* client);

class Server
{
private:
	bool quit = false;

	ENetAddress address;
	ENetHost* peer;
	std::vector<NetworkConnection*> clients;
	ENetEvent serverEvent;

public:
	Server()
	{
		std::cout << "Starting program in server mode!" << std::endl;
	}

	void StartServer()
	{
		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */
		address.host = ENET_HOST_ANY;
		/* Bind the server to port 1234. */
		address.port = 1234;

		peer = enet_host_create(&address /* the address to bind the server host to */,
			32      /* allow up to 32 clients and/or outgoing connections */,
			2      /* allow up to 2 channels to be used, 0 and 1 */,
			0      /* assume any amount of incoming bandwidth */,
			0      /* assume any amount of outgoing bandwidth */);

		if (peer == NULL)
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet server host.\n");
			exit(EXIT_FAILURE);
		}
	}

	void ServerLoop()
	{
		NetworkConnection* newConnection;
		std::string packetString;
		while (enet_host_service(peer, &serverEvent, 1000) > 0 || !quit)
		{
			//PingClients();

			switch (serverEvent.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("A new client connected from %x:%u.\n",
					serverEvent.peer->address.host,
					serverEvent.peer->address.port);
				/* Store any relevant client information here. */
				break;
			case ENET_EVENT_TYPE_RECEIVE:
			{
				Packet receivedPacket((char*)serverEvent.packet->data);
				ParsePacket(&receivedPacket);
			}
				/*for (int i = 0; i < clients.size(); i++)
				{
					std::string message((char*)serverEvent.packet->data);
					SendPacketToClient(clients[i]->connection, message);
				}*/

				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(serverEvent.packet);
				break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", serverEvent.peer->data);
				/* Reset the peer's client information. */
				serverEvent.peer->data = NULL;
			}
		}
	}

	NetworkConnection* GetConnectionFromID(const int ID)
	{
		for (int i = 0; i < clients.size(); i++)
		{
			if (clients.at(i)->m_iID == ID)
			{
				return clients.at(i);
			}
		}

		return nullptr;
	}

	void PingClients()
	{
		for (int i = 0; i < clients.size(); i++)
		{
			SendPacketToClient(clients[i]->m_connection, Packet(-1, 0, std::string("")));
		}
	}

	void SendPacketToClient(ENetPeer* connection, Packet& packetData)
	{
		std::string sendPacket = packetData.m_packetString;
		/* Create a reliable packet of size 7 containing "packet\0" */
		ENetPacket* packet = enet_packet_create(sendPacket.c_str(),
			strlen(sendPacket.c_str()) + 1,
			ENET_PACKET_FLAG_RELIABLE);

		/* Send the packet to the peer over channel id 0. */
		/* One could also broadcast the packet by         */
		/* enet_host_broadcast (host, 0, packet);         */
		enet_peer_send(connection, 0, packet);
		//sends queued packets to the host
		enet_host_flush(peer);
	}

	void CloseServer()
	{
		enet_host_destroy(peer);
	}


	void ParsePacket(Packet* packet)
	{
		NetworkConnection* targetConnection = GetConnectionFromID(packet->m_connectionID);
		switch (packet->m_packetType)
		{
		case 0:
			//ping
			break;
		case 1:
			//ID confirmation
			break;
		case 2:
			//username confirmation
		{
			std::cout << "Received username>: " << packet->m_packetData << std::endl;
			{
				NetworkConnection* newConnection = new NetworkConnection();
				newConnection->m_connection = serverEvent.peer;
				newConnection->m_sUsername = packet->m_packetData;
				clients.push_back(newConnection);
				{
					int newID = clients.size();
					SendPacketToClient(newConnection->m_connection, Packet(newID, 1, std::string("IDConf")));
					newConnection->m_iID = newID;
				}
				std::string messageToBroadcast = newConnection->m_sUsername + " has connected to the server";
				for (int i = 0; i < clients.size(); i++)
				{
					SendPacketToClient(clients.at(i)->m_connection, Packet(-1, 3, messageToBroadcast));
				}
			}
			break;
		}
		case 3:
			//message
			if (strcmp(packet->m_packetData.c_str(), "") == 0)
				break;

			std::string messageToBroadcast = targetConnection->m_sUsername + ": " + packet->m_packetData;
			std::cout << messageToBroadcast << std::endl;
			for (int i = 0; i < clients.size(); i++)
			{
				SendPacketToClient(clients.at(i)->m_connection, Packet(-1, 3, messageToBroadcast));
			}
			break;
		}
	}
};

class Client
{
private:
	ENetHost* client;
	ENetEvent m_clientEvent;

	int clientID;
	bool quit = false;

	ClientParser* clientParser;
public:

	NetworkConnection* clientConnection;

	Client()
	{
		clientLock.lock();
		std::cout << "Started program in client mode!" << std::endl;
		std::cout << "Please enter your name >: ";
		clientConnection = new NetworkConnection();
		std::cin >> clientConnection->m_sUsername;

		clientParser = new ClientParser(clientConnection);
	}

	void ConnectClient()
	{
		ENetAddress address;

		client = enet_host_create(NULL /* create a client host */,
			1 /* only allow 1 outgoing connection */,
			2 /* allow up 2 channels to be used, 0 and 1 */,
			0 /* assume any amount of incoming bandwidth */,
			0 /* assume any amount of outgoing bandwidth */);
		if (client == NULL)
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet client host.\n");
			exit(EXIT_FAILURE);
		}

		enet_address_set_host(&address, "84.69.36.105"); //sets the connection address
		address.port = 1234;
		/* Initiate the connection, allocating the two channels 0 and 1. */
		clientConnection->m_connection = enet_host_connect(client, &address, 2, 0);
		if (clientConnection->m_connection == NULL)
		{
			fprintf(stderr,
				"No available peers for initiating an ENet connection.\n");
			exit(EXIT_FAILURE);
		}

		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(client, &m_clientEvent, 5000) > 0 &&
			m_clientEvent.type == ENET_EVENT_TYPE_CONNECT)
		{
			puts("Connection to server succeeded.");
			Packet packet(clientConnection->m_iID, 2, clientConnection->m_sUsername);
			SendPacket(&packet);
		}
		else
		{
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(clientConnection->m_connection);
		}
	}

	void ClientLoop()
	{
		ENetEvent clientEvent;
		std::string packetInfo;
		clientLock.unlock();
		std::thread messageThread = std::thread(MessageServerLoop, this);
		while (enet_host_service(client, &clientEvent, 1000) > 0 || !quit)
		{
			switch (clientEvent.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
					clientEvent.packet->dataLength,
					clientEvent.packet->data,
					clientEvent.peer->data,
					clientEvent.channelID);
				

				Packet receivedPacket((char*)clientEvent.packet->data);
				clientParser->ParsePacket(&receivedPacket);
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(clientEvent.packet);

				break;
			}
		}
	}

	void SendPacket(Packet* packetInfo)
	{
		std::string packetString = packetInfo->m_packetString;
		/* Create a reliable packet of size 7 containing "packet\0" */
		ENetPacket* packet = enet_packet_create(packetString.c_str(),
			strlen(packetString.c_str()) + 1,
			ENET_PACKET_FLAG_RELIABLE);
		/* Send the packet to the peer over channel id 0. */
		/* One could also broadcast the packet by         */
		/* enet_host_broadcast (host, 0, packet);         */
		enet_peer_send(clientConnection->m_connection, 0, packet);
		//sends queued packets to the host
		enet_host_flush(client);
	}

	void CloseConnection()
	{
		enet_peer_disconnect(clientConnection->m_connection, 0);
		/* Allow up to 3 seconds for the disconnect to succeed
		 * and drop any packets received packets.
		 */
		while (enet_host_service(client, &m_clientEvent, 3000) > 0)
		{
			switch (m_clientEvent.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(m_clientEvent.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				puts("Disconnection succeeded.");
				return;
			}
		}
		enet_host_destroy(client);
	}

	/*void ParsePacket(Packet& packet, ClientConnection* connection)
	{
		switch (packet.m_packetType)
		{
		case 0:
			//ping
			break;
		case 1:
			//ID confirmation
			if (connection != nullptr)
			{
				connection->ID = clientID;
				std::cout << "Received client ID from server! We are: " << connection->ID << std::endl;
			}
			break;
		case 2:
			//username confirmation
		{
			char username[80];
			sscanf(packet.m_packetData, "%*d|%*d|%[^\n]", &username);
			std::string usernameString = username;
			std::cout << "Received username>: " << usernameString << std::endl;
			break;
		}
		case 3:
			//message
			char message[1000];
			sscanf(packet.m_packetData, "%*d|%*d|%[^\n]", &message);
			std::string messageString = message;
			std::cout << messageString << std::endl;
			break;
		}
	}*/
};

int main()
{
	std::cout << "Network test!" << std::endl;

	if (enet_initialize() != 0)
	{
		std::cout << "An error occurred while initializing enet!" << std::endl;
		return EXIT_FAILURE;
	}

	int programMode = 0;
	std::cout << "Enter 1 to start up in Server or 2 for Client Mode >: ";
	std::cin >> programMode;

	if (programMode == 1)
	{
		Server* server = new Server();
		server->StartServer();
		server->ServerLoop();
	}
	else if(programMode == 2)
	{
		Client* client = new Client();
		client->ConnectClient();
		client->ClientLoop();
		client->CloseConnection();
	}

	enet_deinitialize();
	return 0;
}

void MessageServerLoop(Client* client)
{
	while (true)
	{
		clientLock.lock();
		std::string message;
		std::getline(std::cin, message);
		if (message.empty())
		{
			clientLock.unlock();
			continue;
		}
		Packet packet(client->clientConnection->m_iID, 3, message);
		client->SendPacket(&packet);
		clientLock.unlock();
	}
}