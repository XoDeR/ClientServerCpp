#pragma once

#include "NetCommon.h"
#include "NetTsQueue.h"
#include "NetMessage.h"

namespace ClientServerCpp
{
	namespace Net
	{
		template <typename T>
		class Connection : public std::enable_shared_from_this<Connection<T>>
		{
		public:
			Connection()
			{

			}

			virtual ~Conncetion()
			{

			}

		public:
			bool connectToServer()
			{

			}

			bool getIsConnected() const
			{

			}

			bool disconnect()
			{

			}

		public:
			bool send(const Message<T>& msg)

		protected:
			asio::ip::tcp::socket socket;

			asio::io_connect& asioContext;

			TsQueue<Message<T>> messagesOut;

			TsQueue<OwnedMessage> messagesIn;
		};
	}
}