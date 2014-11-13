#include <iostream>
#include <functional>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include "Threadpool.h"
using namespace std;

void gentest(vector<string>& pvals, vector<string>& rules);

typedef function<void(vector<string>&, vector<string>&)> fnc;
typedef vector<string> strvec;

mutex mtx;
int main(int argc, char const *argv[])
{

	fnc g = &gentest;

	strvec data;
	strvec new_data;
	data.push_back("yo");
	data.push_back("test");

	Threadpool<fnc, strvec, strvec> pool(8); // Allocate 8 threads
	
	pool.set_function(g);
	pool.set_input(data);
	pool.set_output(new_data);

	pool.execute_no_atomic();
	pool.join();

	//pool.count_workers(); // check how many threads are active
	// pool.join(); // wait for the threads to finish

	return 0;
}

void gentest(vector<string>& input, vector<string>& output){
	mtx.lock();
	cout << input[0] << endl;
	mtx.unlock();
}