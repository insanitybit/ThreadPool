#include <iostream>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include "Threadpool.h"

#include <chrono>

using namespace std;

void gentest(string&, vector<string>& new_data, const size_t i);
void test(string&, vector<string>& new_data);

typedef function<void(string&, vector<string>&, size_t)> fnc;
typedef function<void(string&, vector<string>&)> ofnc;
typedef vector<string> strvec;

mutex mtx;
int main(int argc, char const *argv[])
{
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<double> elapsed_seconds;

	fnc g = &gentest;
	ofnc f = &test;

	strvec data(1000, "new_string");
	strvec new_data;
	new_data.resize(data.size());

	
	size_t core_count = thread::hardware_concurrency();
	
	// elapsed_seconds = end-start;
	// cout << "no_pool::  " << elapsed_seconds.count() * 1000 << "ms" << endl;


	start = std::chrono::steady_clock::now();


	Threadpool<ofnc, vector<string>, vector<string>> opool(core_count);

	opool.set_function(f);
	opool.execute_no_atomic(ref(data), ref(new_data));
	opool.join();

	end = std::chrono::steady_clock::now();
	
	elapsed_seconds = end-start;
	cout << "execute_no_atomic:\t" << elapsed_seconds.count() * 1000 << "ms" << endl;
	
	start = std::chrono::steady_clock::now();

	cout << "opool\t" << opool.get_items_processed() << endl;

	Threadpool<fnc, vector<string>, vector<string>> pool(core_count);

	pool.set_function(g);
	pool.execute_atomic(ref(data), ref(new_data));

	size_t c;
	while(true)
	{
		c = pool.get_items_processed();
		cout << "pool\t" << c << endl;
		if(c == 1000)
			break;
	}
	pool.join();

	end = std::chrono::steady_clock::now();
	
	
	elapsed_seconds = end-start;
	cout << "execute_atomic::\t" << elapsed_seconds.count() * 1000 << "ms" << endl;

	cout << new_data[100];
	return 0;
}

void gentest(string&, vector<string>& output, const size_t i){

	// std::chrono::milliseconds dura( 500 );

	// std::this_thread::sleep_for(dura);
	// string i = "3";
	// if(input[0] == output[0] )
	// 	i = "5";
	// int matrix[1000][1000];

		// int matrix[100][100];

		// for (int i = 0; i < 100; ++i)
		// {
		// 	for (int j = 0; j < 100; ++j)
		// 	{
		// 		matrix[i][j] = i + j;
		// 	}
		// }
	int it = 1 + 1;

		output[i] = ("i" + to_string(it * 2));

}

void test(string&, vector<string>& output){
	// string i = "3";
	// if(input[0] == output[0] )
	// 	i = "5";
	// int matrix[1000][1000];

		// int matrix[100][100];

		// for (int i = 0; i < 100; ++i)
		// {
		// 	for (int j = 0; j < 100; ++j)
		// 	{
		// 		matrix[i][j] = i + j;
		// 	}
		// // }
		mtx.lock();
		int it = 1 + 1;
		// output.push_back("i");
		mtx.unlock();
}