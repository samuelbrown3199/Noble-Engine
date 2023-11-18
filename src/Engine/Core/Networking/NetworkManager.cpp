#include "NetworkManager.h"
#include "../Core/Logger.h"

NetworkManager::NetworkManager(const int& _type)
{
	if (enet_initialize() != 0)
	{
		Logger::LogError("Failed to initialize enet.", 2);
		return;
	}
	Logger::LogInformation("Initialized enet.");

	if (_type == 0)
		m_parser = new Server();
	else
	{
		std::string username = FormatString("temp%d", rand() % 100000);
		m_parser = new Client(username);
	}
}

NetworkManager::~NetworkManager()
{
	delete m_parser;
	enet_deinitialize();
	Logger::LogInformation("De-initialized enet.");
}