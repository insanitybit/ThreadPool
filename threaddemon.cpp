#include "Threadpool.h"
#include <iostream>
#include <array>
#include <mutex>

using namespace std;

void empty_fake_work();
void args_fake_work(char, int&, string&);
void args_atomic_fake_work(size_t, std::array<size_t,10>& ,std::array<size_t,10>&);

mutex mtx;

int main()
{

	array<size_t, 10> data;
	data.fill(10);
	array<size_t, 10> new_data;

	const size_t run_count = 10;
	const auto core_count = thread::hardware_concurrency();


	/*
		Example of threadpool that executes a function that requires no arguments.
	*/


	auto empty_f = &empty_fake_work;

	Threadpool<decltype(empty_f)> voidpool(core_count, empty_f);
	voidpool.execute_no_atomic(run_count);
	voidpool.join();


	/*
		Example of a threadpool that uses a variadic template to take in any number of arguments
		The threadpool will pass these arguments to the passed function
	*/


	auto args_f = &args_fake_work;
	char a = 'a';
	int x = 5;
	string str = "old";

	Threadpool<decltype(args_f), char, int, string> argspool(core_count, args_f);
	argspool.execute_no_atomic(run_count, a, x, str);
	argspool.join();
	// str = "New ", x = 15


	/*
		Another variadic threadpool, but this one passes in an atomic for synchronization
	*/

	auto atomic_f = &args_atomic_fake_work;

	Threadpool<decltype(atomic_f), decltype(data), decltype(new_data) > atompool(core_count, atomic_f);
	atompool.execute_atomic(data.size(), std::ref(data), std::ref(new_data));
	atompool.join();

	// for(auto&& a : new_data)
	// 	cout << a << endl;

	return 0;
}

void empty_fake_work(){
	// cout << "empty fake work\n";
}

void args_fake_work(char a, int& x, string& s){
	mtx.lock();
	// cout << "args fake work\n";
	s = "New ";
	x = 15;
	mtx.unlock();
	a = 'b';
}

void args_atomic_fake_work(size_t it, std::array<size_t, 10>& input ,std::array<size_t, 10>& output){

	output[it] = input[it] + it;

}