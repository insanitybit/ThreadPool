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
		void set_input(R&);
		void set_output(S&);
		S get_output();
		void execute_no_atomic();
		void execute_atomic();
		void join();
	//	~Threadpool();

	private:
		void thread_exec(std::atomic<size_t>& it);
		void thread_exec_i(std::atomic<size_t>& it);
		size_t thread_count;
		size_t active_count;
		std::vector<std::thread> threads;
		R input;
		S output;
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
void Threadpool<T,R,S>::set_input(R& i){
	input = i;
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::set_output(S& o){
	output = o;
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::execute_no_atomic(){
std::atomic<size_t> it;

	for (int i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec, this, std::ref(it));
	}
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::thread_exec(std::atomic<size_t>& it){
	
	while(it < input.size()) {
		size_t currentIndex = it++;

		if(currentIndex >= input.size()) {
			break;
		}

		fn(std::ref(input[currentIndex]), std::ref(output));
	}

}

template<class T, class R, class S> 
void Threadpool<T,R,S>::execute_atomic(){
	std::atomic<size_t> it;

	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec_i, this, std::ref(it));
	}
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::thread_exec_i(std::atomic<size_t>& it){
	while(it < input.size()) {
		size_t currentIndex = it++;

		if(currentIndex >= input.size()) {
			break;
		}

		fn(std::ref(input[it]), std::ref(output), currentIndex);
	}
	
}

template<class T, class R, class S> 
void Threadpool<T,R,S>::join(){

	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i].join();
	}
}

template<class T, class R, class S> 
S Threadpool<T,R,S>::get_output(){

	return output;
}

// template<class T, class R, class S> 
// Threadpool<T,R,S>::~Threadpool(){
// 	threads.clear();
// }
#endif