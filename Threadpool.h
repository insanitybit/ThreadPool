#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <iostream>
#include <functional>
#include <algorithm>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <atomic> 

// T = function
// R = container to store output
// S = data to be acted upon
template<class T, class R, class S>
class Threadpool
{
	public:
		Threadpool(size_t count);
		void set_function(T);
		void execute_no_atomic(R&, S&);
		void execute_atomic(R&, S&);
		void join();
		~Threadpool();

	private:
		void thread_exec(R&, S&, std::atomic<size_t>& it);
		void thread_exec_i(R&, S&, std::atomic<size_t>& it);
		size_t thread_count;
		size_t active_count;
		std::vector<std::thread> threads;
		T fn;
		std::mutex mtx;
		bool t;
};

template<class T, class R, class S> 
Threadpool<T,R,S>::Threadpool(size_t count){
	threads.resize(count);
	thread_count = count;
	active_count = 0;

}

template<class T, class R, class S> 
void Threadpool<T,R,S>::set_function(T f){
	fn = f;
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::execute_no_atomic(R& input, S& output){
std::atomic<size_t> it;
	it = 0;
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec, this, std::ref(input), std::ref(output), std::ref(it));
	}
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::thread_exec(R& input, S& output, std::atomic<size_t>& it){
	
	while(it < input.size()) {
		size_t currentIndex = it++;

		if(currentIndex >= input.size()) {
			break;
		}

		fn(std::ref(input[currentIndex]), std::ref(output));
	}

}

template<class T, class R, class S> 
void Threadpool<T,R,S>::execute_atomic(R& input, S& output){
	std::atomic<size_t> it;
	it = 0;

	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec_i, this, std::ref(input), std::ref(output), std::ref(it));
	}
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::thread_exec_i(R& input, S& output, std::atomic<size_t>& it){
	while(it < input.size()) {
		size_t currentIndex = it++;

		if(currentIndex >= input.size()) {
			break;
		}

		fn(std::ref(input[it]), std::ref(output), it);
	}
	
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::join(){

	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i].join();
	}
}

// template<class T, class R, class S> 
// S Threadpool<T,R,S>::get_output(){

// 	return output;
// }

template<class T, class R, class S> 
Threadpool<T,R,S>::~Threadpool(){
	threads.clear();
}
#endif