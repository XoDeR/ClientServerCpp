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

class CustomClient : public ClientServerCpp::Net::ClientInterface<CustomMsgTypes>
{
public:
	void pingServer()
	{
		ClientServerCpp::Net::Message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();

		msg << timeNow;
		send(msg);
	}
};

int main()
{
	CustomClient c;
	c.connect("127.0.0.1", 60000);

	bool key[3] = { false, false, false };
	bool oldKey[3] = { false, false, false };

	bool quit = false;
	while (!quit)
	{
		if (GetForegroundWindow() == GetConsoleWindow())
		{
			key[0] = GetAsyncKeyState('1') & 0x8000;
			key[1] = GetAsyncKeyState('2') & 0x8000;
			key[2] = GetAsyncKeyState('3') & 0x8000;
		}

		if (key[0] && !oldKey[0])
		{
			c.pingServer();
		}
		if (key[2] && !oldKey[2])
		{
			quit = true;
		}

		for (int i = 0; i < 3; ++i)
		{
			oldKey[i] = key[i];
		}

		if (c.getIsConnected())
		{
			if (!c.getIncoming().getIsEmpty())
			{
				auto msg = c.getIncoming().popFront().msg;

				switch (msg.header.id)
				{
				case CustomMsgTypes::ServerPing :
				{
					std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
					std::chrono::system_clock::time_point timeThen;
					msg >> timeThen;

					std::cout << "Ping: " << std::chrono::duration<double>(timeNow - timeThen).count() << "\n";
				}
				break;
				}
			}
		}
		else
		{
			std::cout << "Server down\n";
			quit = true;
		}
	}

	return 0;
}