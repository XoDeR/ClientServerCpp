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
		class ServerInterface
		{
		public:
			ServerInterface(uint16_t port)
				: asioAcceptor(asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{

			}

			virtual ~ServerInterface()
			{
				stop();
			}

			bool start()
			{
				try
				{
					waitForClientConnection();

					threadContext = std::thread([this]() { asioContext.run(); });
				}
				catch (std::exception& e)
				{
					std::cerr << "[Server] exception: " << e.what() << "\n";
					return false;
				}

				std::cout << "[Server] started!\n";
				return true;
			}

			void stop()
			{
				asioContext.stop();

				if (threadContext.joinable())
				{
					threadContext.join();
				}

				std::cout << "[Server] stopped!\n"
			}

			void waitForClientConnection()
			{
				asioAcceptor.async_accept(
					[this](std::error_code ec, asio::ip::tcp::socket socket) 
					{
						if (!ec)
						{
							std::cout << "[Server] New connection: " << socket.remote_endpoint() << "\n";

							/*
							std::shared_ptr<Connection<T>> newConnection = std::make_shared<Connection<T>>(Connection<T>::owner::server,
								asioContext, std::move(socket), messagesIn);
								*/

							//if (onClientConnect(newConnection))
							//{
							//	dequeConnections.push_back(std::move(newConnection));
							//	dequeConnections.back()->connectToClient(idCounter++);
							//	std::cout << "[" << dequeConnections.back()->getId() << "] Connection approved\n";
							//}
							//else
							//{
							//	std::cout << "[-----] Connection denied\n";
							//}
						}
						else
						{
							std::cout << "[Server] New connection error: " << ec.message() << "\n";
						}

						waitForClientConnection();
					}
				);
			}

			void messageClient(std::shared_ptr<Connection<T>> client, const Message<T>& msg)
			{
				if (client && client->getIsConnected())
				{
					client->send(msg);
				}
				else
				{
					onClientDisconnect(client);
					client.reset();
					dequeConnections.erase(
						std::remove(dequeConnections.begin(), dequeConnections.end(), client), dequeConnections.end());
				}
			}

			void messageAllClients(const Message<T>& msg, std::shared_ptr<Connection<T>> ignoreClient = nullptr)
			{
				bool invalidClientExists = false;

				for (auto& client : dequeConnections)
				{
					if (client && client->getIsConnected())
					{
						if (client != ignoreClient)
						{
							client->send(msg);
						}
					}
					else
					{
						onClientDisconnect(client);
						client.reset();
						invalidClientExists = true;
					}
				}

				if (invalidClientExists == true)
				{
					dequeConnections.erase(
						std::remove(dequeConnections.begin(), dequeConnections.end(), nullptr), dequeConnections.end());
				}
			}

			void update(size_t maxMessages = -1)
			{
				size_t messageCount = 0;
				while (messageCount < maxMessages && !messagesIn.getIsEmpty())
				{
					auto msg = messagesIn.popFront();
					onMessage(msg.remote, msg.msg);
					messageCount++;
				}
			}

		protected:

			virtual bool onClientConnect(std::shared_ptr<Connection<T>> client)
			{
				return false;
			}

			virtual void onClientDisconnect(std::shared_ptr<Connection<T>> client)
			{

			}

			virtual void onMessage(std::shared_ptr<Connection<T>> client, Message<T>& msg)
			{

			}

		protected:

			TsQueue<OwnedMessage<T>> messagesIn;
			std::deque<std::shared_ptr<Connection<T>>> dequeConnections;
			asio::io_context asioContext;
			std::thread threadContext;

			asio::ip::tcp::acceptor asioAcceptor;

			uint32_t idCounter = 10000;
		};
	}
}