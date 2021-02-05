#include <iostream>

#include "Net.h"

enum class CustomMsgTypes : uint32_t
{
	FireBullet,
	MovePlayer
};

class CustomClient : public ClientServerCpp::Net::ClientInterface<CustomMsgTypes>
{
public:
	bool fireBullet(float x, float y)
	{
		ClientServerCpp::Net::Message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::FireBullet;
		msg << x << y;
		send(msg);
	}
};

int main()
{
	CustomClient c;
	c.connect("community.onelonecoder.com", 60000);
	c.fireBullet(2.0f, 5.0f);


	//ClientServerCpp::Net::Message<CustomMsgTypes> msg;
	//msg.header.id = CustomMsgTypes::FireBullet;

	//int a = 1;
	//bool b = true;
	//float c = 3.14159f;

	//struct 
	//{
	//	float x;
	//	float y;
	//} d[5];

	//msg << a << b << c << d;

	//a = 99;
	//b = false;
	//c = 99.0f;

	//msg >> d >> c >> b >> a;

	return 0;
}