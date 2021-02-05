#pragma once

#include "NetCommon.h"

namespace ClientServerCpp
{
	namespace Net
	{
		template<typename T>
		class TsQueue
		{
		public:
			TsQueue() = default;
			TsQueue(const TsQueue<T>& tsQueue) = delete;
			virtual ~TsQueue()
			{
				clear();
			}
		public:
			const T& front()
			{
				std::scoped_lock lock(mutexQueue);
				return dequeQueue.front();
			}

			const T& back()
			{
				std::scoped_lock lock(mutexQueue);
				return dequeQueue.back();
			}

			void pushBack(const T& item)
			{
				std::scoped_lock lock(mutexQueue);
				dequeQueue.emplace_back(std::move(item));
			}

			void pushFront(const T& item)
			{
				std::scoped_lock lock(mutexQueue);
				dequeQueue.emplace_front(std::move(item));
			}

			bool getIsEmpty()
			{
				std::scoped_lock lock(mutexQueue);
				return dequeQueue.empty();
			}

			size_t getCount()
			{
				std::scoped_lock lock(mutexQueue);
				return dequeQueue.size();
			}

			void clear()
			{
				std::scoped_lock lock(mutexQueue);
				dequeQueue.clear();
			}

			T popFront()
			{
				std::scoped_lock lock(mutexQueue);
				auto t = std::move(dequeQueue.front());
				dequeQueue.pop_front();
				return t;
			}

			T popBack()
			{
				std::scoped_lock lock(mutexQueue);
				auto t = std::move(dequeQueue.back());
				dequeQueue.pop_back();
				return t;
			}
		protected:
			std::mutex mutexQueue;
			std::deque<T> dequeQueue;
		};
	}
}