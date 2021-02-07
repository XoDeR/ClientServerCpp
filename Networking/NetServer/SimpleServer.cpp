#include <iostream>
#include "Net.h"

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage
};

class CustomServer : public ClientServerCpp::Net::ServerInterface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t port) 
		: ClientServerCpp::Net::ServerInterface<CustomMsgTypes>(port)
	{

	}
protected:

	virtual bool onClientConnect(std::shared_ptr<ClientServerCpp::Net::Connection<CustomMsgTypes>> client)
	{
		return true;
	}

	virtual void onClientDisconnect(std::shared_ptr<ClientServerCpp::Net::Connection<CustomMsgTypes>> client)
	{
	}

	virtual void onMessage(std::shared_ptr<ClientServerCpp::Net::Connection<CustomMsgTypes>> client, ClientServerCpp::Net::Message<CustomMsgTypes>& msg)
	{
		switch (msg.header.id)
		{
		case CustomMsgTypes::ServerPing:
		{
			std::cout << "[" << client->getId() << "] Server ping\n";
			client->send(msg);
		}
		break;
		}
	}
};

int main()
{
	CustomServer customServer(60000);
	customServer.start();

	while (true)
	{
		customServer.update();
	}
	return 0;
}