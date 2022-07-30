#pragma once
#ifndef PACKETPARSER_H_
#define PACKETPARSER_H_

#include <enet/enet.h>

#include <thread>

#include "../Core/Logger.h"
#include "../Useful.h"
#include "Packet.hpp"

class PacketParser
{
public:
	virtual void ParsePacket(Packet* _packet) = 0;
	virtual void StartConnection() = 0;
	virtual void Loop() = 0;
	virtual void CloseConnection() = 0;

	void SendPacket(Packet* _packetInfo, ENetPeer* _connection, ENetHost* _host)
	{
		std::string packetString = _packetInfo->m_packetString;
		/* Create a reliable packet of size 7 containing "packet\0" */
		ENetPacket* packet = enet_packet_create(packetString.c_str(),
			strlen(packetString.c_str()) + 1,
			ENET_PACKET_FLAG_RELIABLE);
		/* Send the packet to the peer over channel id 0. */
		/* One could also broadcast the packet by         */
		/* enet_host_broadcast (host, 0, packet);         */
		enet_peer_send(_connection, 0, packet);
		//sends queued packets to the host
		enet_host_flush(_host);
	}
};

class Client : public PacketParser
{

private:

	ENetHost* m_client;
	NetworkConnection* m_clientConnection;
	ENetEvent m_clientEvent;

public:

	Client(const std::string& _username)
	{
		m_clientConnection = new NetworkConnection();
		m_clientConnection->m_sUsername = _username;

		StartConnection();
	}

	void StartConnection()
	{
		ENetAddress address;

		m_client = enet_host_create(NULL /* create a client host */,
			1 /* only allow 1 outgoing connection */,
			2 /* allow up 2 channels to be used, 0 and 1 */,
			0 /* assume any amount of incoming bandwidth */,
			0 /* assume any amount of outgoing bandwidth */);
		if (m_client == NULL)
		{
			Logger::LogError("An error occurred while trying to create an ENet client host.", 0);
		}

		enet_address_set_host(&address, "90.241.158.48"); //sets the connection address
		address.port = 1234;
		/* Initiate the connection, allocating the two channels 0 and 1. */
		m_clientConnection->m_connection = enet_host_connect(m_client, &address, 2, 0);
		if (m_clientConnection->m_connection == NULL)
		{
			Logger::LogError("No available peers for initiating an ENet connection.", 0);
		}

		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(m_client, &m_clientEvent, 5000) > 0 &&
			m_clientEvent.type == ENET_EVENT_TYPE_CONNECT)
		{
			Logger::LogInformation("Connection to server succeeded.");
			Packet packet(m_clientConnection->m_iID, 2, m_clientConnection->m_sUsername);
			SendPacket(&packet, m_clientConnection->m_connection, m_client);
		}
		else
		{
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(m_clientConnection->m_connection);
		}
	}

	void CloseConnection()
	{
		enet_peer_disconnect(m_clientConnection->m_connection, 0);
		/* Allow up to 3 seconds for the disconnect to succeed
		 * and drop any packets received packets.
		 */
		while (enet_host_service(m_client, &m_clientEvent, 3000) > 0)
		{
			switch (m_clientEvent.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(m_clientEvent.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				Logger::LogInformation("Disconnection succeeded.");
				return;
			}
		}
		enet_host_destroy(m_client);
	}

	void Loop()
	{
		Logger::LogInformation("Started client loop.");
		std::string packetInfo;

		std::thread* messageThread = new std::thread([=] { MessageServerLoop(this); });

		while (enet_host_service(m_client, &m_clientEvent, 1000) > 0 || true)
		{
			switch (m_clientEvent.type)
			{
			case ENET_EVENT_TYPE_RECEIVE:
				Logger::LogInformation(FormatString("A packet of length %u containing %s was received from %s on channel %u.",
					m_clientEvent.packet->dataLength,
					m_clientEvent.packet->data,
					m_clientEvent.peer->data,
					m_clientEvent.channelID));


				Packet receivedPacket((char*)m_clientEvent.packet->data);
				ParsePacket(&receivedPacket);
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(m_clientEvent.packet);

				break;
			}
		}
	}

	void ParsePacket(Packet* packet)
	{
		switch (packet->m_packetType)
		{
		case 0:
			//ping
			break;
		case 1:
			//ID confirmation
			if (m_clientConnection != nullptr)
			{
				m_clientConnection->m_iID = packet->m_connectionID;
				std::cout << "Received client ID from server! We are: " << m_clientConnection->m_iID << std::endl;
			}
			break;
		case 2:
			//username confirmation
		{
			std::string usernameString = packet->m_packetData;
			std::cout << "Received username>: " << usernameString << std::endl;
			break;
		}
		case 3:
			//message
		{
			std::string messageString = packet->m_packetData;
			std::cout << messageString << std::endl;
			break;
		}
		default:
			Logger::LogError(FormatString("Received an unknown packet type from the server, with the following data>: %s", packet->m_packetData), 0);
			break;
		}
	}

	void MessageServerLoop(Client* client)
	{
		while (true)
		{
			std::string message;
			std::getline(std::cin, message);
			if (message.empty())
			{
				continue;
			}
			Packet packet(client->m_clientConnection->m_iID, 3, message);
			client->SendPacket(&packet, m_clientConnection->m_connection, m_client);
		}
	}
};

/*
* Parser for a server host.
*/
class Server : public PacketParser
{
private:

	ENetAddress _address;
	ENetHost* m_peer;
	std::vector<NetworkConnection*> clients;
	ENetEvent serverEvent;

public:
	
	Server()
	{
		StartConnection();
	}

	void StartConnection()
	{
		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */
		_address.host = ENET_HOST_ANY;
		/* Bind the server to port 1234. */
		_address.port = 1234;

		m_peer = enet_host_create(&_address /* the address to bind the server host to */,
			32      /* allow up to 32 clients and/or outgoing connections */,
			2      /* allow up to 2 channels to be used, 0 and 1 */,
			0      /* assume any amount of incoming bandwidth */,
			0      /* assume any amount of outgoing bandwidth */);

		if (m_peer == NULL)
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet server host.\n");
			exit(EXIT_FAILURE);
		}
	}

	void CloseConnection()
	{
		enet_host_destroy(m_peer);
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

	void Loop()
	{
		Logger::LogInformation("Started server loop.");

		NetworkConnection* newConnection;
		std::string packetString;
		while (enet_host_service(m_peer, &serverEvent, 1000) > 0 || true)
		{
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
			std::string messageToBroadcast = targetConnection->m_sUsername + ": " + packet->m_packetData;
			std::cout << messageToBroadcast << std::endl;
			for (int i = 0; i < clients.size(); i++)
			{
				SendPacketToClient(clients.at(i)->m_connection, Packet(-1, 3, messageToBroadcast));
			}
			break;
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
		enet_host_flush(m_peer);
	}
};

#endif