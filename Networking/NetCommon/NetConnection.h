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
			enum class owner
			{
				server,
				client
			};

		public:
			Connection(owner parent, asio::io_context& asioContext, asio::ip::tcp::socket socket, TsQueue<OwnedMessage<T>>& qIn)
				: asioContext(asioContext), socket(std::move(socket)), messagesIn(qIn)
			{
				ownerType = parent;
			}

			virtual ~Connection()
			{

			}

			uint32_t getId() const
			{
				return id;
			}

		public:
			void connectToClient(uint32_t uid = 0)
			{
				if (ownerType == owner::server)
				{
					if (socket.is_open())
					{
						id = uid;
						readHeader();
					}
				}
			}

			bool connectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (ownerType == owner::client)
				{
					asio::async_connect(socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{
								readHeader();
							}
						}
					);
				}
			}

			bool getIsConnected() const
			{
				return socket.is_open();
			}

			bool disconnect()
			{
				if (getIsConnected())
				{
					asio::post(asioContext, [this]() { socket.close(); });
				}
			}

			void startListening()
			{

			}

		public:
			bool send(const Message<T>& msg)
			{
				asio::post(asioContext,
					[this, msg]()
					{
						bool writingMessage = !messagesOut.empty();
						messagesOut.push_back(msg);
						if (!writingMessage)
						{
							writeHeader();
						}
					}
				);
			}

		private:

			void writeHeader()
			{
				asio::async_write(socket, asio::buffer(&messagesOut.front().header, sizeof(MessageHeader<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (messagesOut.front().body.size() > 0)
							{
								writeBody();
							}
							else
							{
								messagesOut.pop_front();

								if (!messagesOut.empty())
								{
									writeHeader();
								}
							}
						}
						else
						{
							std::cout << "[" << id << "] writeHeader failure.\n";
							socket.close();
						}
					});
			}

			void writeBody()
			{
				asio::async_write(socket, asio::buffer(messagesOut.front().body.data(), messagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							messagesOut.pop_front();

							if (!messagesOut.empty())
							{
								writeHeader();
							}
						}
						else
						{
							std::cout << "[" << id << "] writeBody failure.\n";
							socket.close();
						}
					});
			}

			void readHeader()
			{
				asio::async_read(socket, asio::buffer(&msgTemporaryIn.header, sizeof(MessageHeader<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							if (msgTemporaryIn.header.size > 0)
							{
								msgTemporaryIn.body.resize(msgTemporaryIn.header.size);
								readBody();
							}
							else
							{
								addToIncomingMessageQueue();
							}
						}
						else
						{
							std::cout << "[" << id << "] readHeader failure.\n";
							socket.close();
						}
					});
			}

			void readBody()
			{
				asio::async_read(socket, asio::buffer(msgTemporaryIn.body.data(), msgTemporaryIn.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							addToIncomingMessageQueue();
						}
						else
						{
							std::cout << "[" << id << "] readBody failure.\n";
							socket.close();
						}
					});
			}

			void addToIncomingMessageQueue()
			{
				if (ownerType == owner::server)
				{
					messagesIn.pushBack({ this->shared_from_this(), msgTemporaryIn });
				}
				else
				{
					messagesIn.pushBack({ nullptr, msgTemporaryIn });
				}

				readHeader();
			}

		protected:
			asio::ip::tcp::socket socket;
			asio::io_context& asioContext;
			TsQueue<Message<T>> messagesOut;
			TsQueue<OwnedMessage<T>>& messagesIn;
			Message<T> msgTemporaryIn;
			owner ownerType = owner::server;
			uint32_t id = 0;
		};
	}
}