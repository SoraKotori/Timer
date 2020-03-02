#include <algorithm>
#include <numeric>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>
#include <functional>
#include <vector>
#include <atomic>

#pragma warning(disable : 4996)

struct exec_wrapper
{
	template<class... Args>
	//auto operator()(Args&&... args) -> decltype(exec(std::forward<Args>(args)...))
	auto operator()(Args&&... args) -> decltype(exec(std::forward<Args>(args)...))
	{
		return exec(std::forward<Args>(args)...);
	}
};

//template<class Clock = std::chrono::steady_clock, class Duration, class Function, class... Args>
//void func(Function&& function, Args&&... args)
//{
//	auto time = Clock::now();
//
//	while (true)
//	{
//		time += Duration();
//		std::this_thread::sleep_until(time);
//
//		function(std::forward<Args>(args)...);
//	}
//}

template<typename Clock = std::chrono::steady_clock, typename Duration = typename Clock::duration>
class timers : public std::thread
{
public:
	using clock = Clock;
	using duration = Duration;

	template<typename Function, typename... Args>
	timers(const duration& interval, std::atomic_bool& exe, Function&& function, Args&&... args) :
		thread(loop,
			std::ref(interval),
			std::forward<std::atomic_bool>(exe),
			std::forward<Function>(function),
			std::forward<Args>(args)...)
	{
	}

private:
	template<class Function, class... Args>
	static auto loop(const duration& interval, std::atomic_bool& exe, Function&& function, Args&&... args)
	{
		auto time = clock::now();

		while (exe)
		{
			time += interval;
			std::this_thread::sleep_until(time);

			std::forward<Function>(function)(std::forward<Args>(args)...);
		}
	}
};

template<typename Duration, typename Duration::rep rep>
struct timer
{
	template<typename Function, typename... Args>
	auto operator()(std::atomic_bool& atomic, Function&& function, Args&&... args)
	{
		auto time = std::chrono::steady_clock::now();

		while (atomic)
		{
			time += Duration(rep);
			std::this_thread::sleep_until(time);

			std::forward<Function>(function)(std::forward<Args>(args)...);
		}
	}
};

template<typename T>
void print_time(T t)
{
	auto start = std::chrono::system_clock::now();
	auto start_time = std::chrono::system_clock::to_time_t(start);
	std::cout << std::put_time(std::localtime(&start_time), "%F %T") << std::endl;
}

int main()
{
	std::atomic_bool atomic = true;
	auto lambda = [](auto&&... args) { return print_time(std::forward<decltype(args)>(args)...); };

	std::thread MyTimer(timer<std::chrono::milliseconds, 250>(), std::ref(atomic), lambda, 3);
	
	std::this_thread::sleep_for(std::chrono::seconds(5));

	atomic = false;
	MyTimer.join();
}