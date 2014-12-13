#include <iostream>
#include <array>
#include <thread>
#include <chrono>

using namespace std;

void fake_work(size_t);

int main()
{

	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;
	start = std::chrono::steady_clock::now();

	array<size_t, 100> data;
	data.fill(10);

	for (size_t i = 0; i < data.size(); ++i)
	{
		fake_work(data[i]);
	}

	end = std::chrono::steady_clock::now();

	elapsed_seconds = end-start;
	cout << "Single thread execution time::\t" << elapsed_seconds.count() * 1000 << "ms" << endl;

	return 0;
}

void fake_work(size_t t){
	std::chrono::milliseconds dura(t);

	std::this_thread::sleep_for(dura);

}