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
};

int main()
{
	CustomClient c;
	c.connect("127.0.0.1", 60000);

	return 0;
}