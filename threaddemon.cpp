#include "Threadpool.h"
#include <iostream>
#include <array>
#include <chrono>

using namespace std;

typedef function<void(size_t)> fnc;

void fake_work(size_t);

int main()
{

	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;
	start = std::chrono::steady_clock::now();

	const size_t core_count = thread::hardware_concurrency();
	fnc f = &fake_work;

	array<size_t, 100> data;
	data.fill(10);

	Threadpool<fnc, array<size_t, 100>> pool(core_count, f);

	pool.execute_no_atomic(std::ref(data));
	pool.join();

	end = std::chrono::steady_clock::now();

	elapsed_seconds = end-start;
	cout << "Threadpool execution time::\t" << elapsed_seconds.count() * 1000 << "ms" << endl;

	return 0;
}

void fake_work(size_t t){
	std::chrono::milliseconds dura(t);

	std::this_thread::sleep_for(dura);

}