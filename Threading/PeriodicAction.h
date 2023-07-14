#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

namespace Threading {
	class PeriodicAction
	{
	public:
		PeriodicAction() = default;
		PeriodicAction(std::chrono::milliseconds initDelay, std::chrono::milliseconds delay, std::function<void()> f);
		~PeriodicAction();

		void Stop();
	private:
		std::mutex m;
		std::condition_variable c;
		bool shouldStop;
		std::function<void()> const f;
		std::chrono::milliseconds const initialDelay;
		std::chrono::milliseconds const delay;
		std::thread t;

		bool wait(std::chrono::milliseconds delay);
		void threadFn();
	};
}