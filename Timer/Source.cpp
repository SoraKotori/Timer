#include <chrono>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <thread>

#pragma warning(disable : 4996)

auto task()
{

}

template<class Clock = std::chrono::steady_clock>
class timer
{

public:
	template<class Duration, class Function, class... Args >
	timer(const Duration& epoch, Function&& function, Args&&... args) :
		m_epoch(epoch),
		//m_thread(std::forward<Function>(function), std::forward<Args>(args)...)
	{
	}

	~timer()
	{
		m_thread.join();
	}

private:
	Clock::duration m_epoch;
	std::thread m_thread;

	void todo()
	{

	}
};


template<
	class Duration,
	class Function, class... Args>
void as(Duration epoch, Function function, Args&&... args)
{
	auto time = std::chrono::steady_clock::now();

	while (true)
	{
		function(std::forward<Args>(args)...);

		time += epoch;
		std::this_thread::sleep_until(time);
	}
}

int main()
{
	auto start = std::chrono::system_clock::now();

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	auto end = std::chrono::system_clock::now();
		
	auto start_time = std::chrono::system_clock::to_time_t(start);
	auto end_time = std::chrono::system_clock::to_time_t(end);
	auto sleep_time = end - start;

	std::cout << std::put_time(std::localtime(&start_time), "%F %T") << std::endl;
	std::cout << std::put_time(std::localtime(&end_time), "%F %T") << std::endl;
	std::cout << sleep_time.count() << std::endl;
}