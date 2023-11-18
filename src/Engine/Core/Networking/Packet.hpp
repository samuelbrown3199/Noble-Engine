#pragma once
#ifndef PACKET_H_
#define PACKET_H_

#include <iostream>
#include <string>

#include "../Useful.h"

/*
* Stores information for a connection over a network.
*/
struct NetworkConnection
{
	size_t m_iID = 0;
	std::string m_sUsername;
	ENetPeer* m_connection;
};

/*
*Used to handle packets of data that will be sent over a network.
*/
struct Packet
{
	int m_connectionID, m_packetType;
	std::string m_packetData;
	std::string m_packetString;

	Packet(std::string packetString)
	{
		if (packetString.empty())
			return;

		m_packetString = packetString;
		std::vector<std::string> splitPacket = SplitString(m_packetString, '|');
		m_connectionID = std::stoi(splitPacket.at(0));
		m_packetType = std::stoi(splitPacket.at(1));
		m_packetData = splitPacket.at(2);
	}

	Packet(const size_t& connectionID, const int& packetType, std::string& packetData)
	{
		m_connectionID = connectionID;
		m_packetType = packetType;
		if (!packetData.empty())
		{
			m_packetString = FormatString("%d|%d|%s", m_connectionID, m_packetType, packetData.c_str());
			m_packetData = packetData;
		}
	}
};

#endif