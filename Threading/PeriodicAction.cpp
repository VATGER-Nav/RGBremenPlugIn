#include "PeriodicAction.h"

Threading::PeriodicAction::PeriodicAction(std::chrono::milliseconds initDelay, std::chrono::milliseconds delay, std::function<void()> f) :
	shouldStop(false),
	f(std::move(f)),
	initialDelay(initDelay),
	delay(delay),
	t(&Threading::PeriodicAction::threadFn, this)
{
}

Threading::PeriodicAction::~PeriodicAction()
{
	this->Stop();
	this->t.join();
}

void Threading::PeriodicAction::Stop()
{
	{
		std::scoped_lock<std::mutex> lock(this->m);
		this->shouldStop = true;
	}

	this->c.notify_one();
}

bool Threading::PeriodicAction::wait(std::chrono::milliseconds delay)
{
	std::unique_lock<std::mutex> lock(this->m);
	this->c.wait_for(lock, delay, [this]() {return this->shouldStop; });
	return !this->shouldStop;
}

void Threading::PeriodicAction::threadFn()
{
	for (auto delay = this->initialDelay; this->wait(delay); delay = this->delay) {
		this->f();
	}
}
