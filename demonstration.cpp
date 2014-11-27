#include "Threadpool.h"
#include <iostream>
#include <mutex>

using namespace std;

typedef function<void(const string&, vector<string>&)> fnc;


void demon(const string&, vector<string>&);

mutex mtx;
int main()
{
	// create our std::function wrapper
	const auto fn = &demon;

	// get CPU cores of system
	const size_t core_count = thread::hardware_concurrency();

	// input values in vector
	const std::vector<string>	input(100, "Hello World");
	std::vector<string> output;

	// Constructor requires type of function, input, and output for template.
	// Pass in however many threads to spawn.
	Threadpool<fnc, const vector<string>, vector<string>> pool(core_count);

	// fn is whichever function will be threaded
	pool.set_function(fn);

	// execute_no_atomic requires that any critical region in the function use a mutex
	pool.execute_no_atomic(cref(input), ref(output));
	
	// wait for results
	pool.join();

	for (auto i = output.begin(); i != output.end(); ++i)
	{
		cout << *i << endl;
	}

	return 0;
}

void demon(const string& str, vector<string>& vec){

	mtx.lock();
	vec.push_back(str);
	mtx.unlock();

}