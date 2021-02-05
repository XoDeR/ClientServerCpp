#pragma once

#include "NetCommon.h"
#include "NetTsQueue.h"
#include "NetMessage.h"
#include "NetConnection.h"

namespace ClientServerCpp
{
	namespace Net
	{
		template <typename T>
		class ClientInterface
		{
		public:
			ClientInterface() 
				: socket(context)
			{

			}

			~ClientInterface()
			{
				disconnect();
			}

			bool connect(const std::string& host, const uint16_t port)
			{
				try
				{
					connection = std::make_unique<Connection<T>>();

					asio::ip::tcp::resolver resolver(context);
					endpoints = resolver.resolve(host, std::to_string(port));

					connction->connectToServer(endpoints);

					thrContext = std::thread([this]() { context.run() });
				}
				catch (std::exception& e)
				{
					std::cerr << "Client exception: " << e.what() << "\n";
					return false;
				}
				return true;
			}

			void disconnect()
			{
				if (getIsConnected())
				{
					connection->disconnect();
				}

				context.stop();

				if (thrContext.joinable())
				{
					thrContext.join();
				}

				connection.release();
			}

			bool getIsConnected()
			{
				if (connection)
				{
					return connection->getIsConnected();
				}
				else
				{
					return false;
				}
			}

			TsQueue<OwnedMessage<T>>& getIncoming()
			{
				return messagesIn;
			}
		protected:
			asio::io_context context;
			std::thread thrContext;
			asio::ip::tcp::socket socket;
			std::unique_ptr<Connection<T>> connection;
		private:
			TsQueue<OwnedMessage<T>> messagesIn;
		};

	}
}