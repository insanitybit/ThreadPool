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
//#include <assert.h>

// T = function
// R = container to store output
// S = data to be acted upon
template<class T, class R,typename... Args>
class Threadpool
{
	public:
		Threadpool(const T&);
		Threadpool(const size_t count);
		Threadpool(const size_t count, const T&);

		void set_function(const T&);
		void execute_no_atomic(R&, Args...);
		void execute_atomic(R&, Args...);
		void join();
		void sleep_all();
		void wake_all();
		size_t get_active_count();
		size_t get_items_processed();
		size_t get_thread_count();

	private:
		void thread_exec(R&, std::atomic<size_t>& it, Args...);
		void thread_exec_i(R&, std::atomic<size_t>& it, Args...);
		size_t thread_count;
		size_t active_count;
		std::vector<std::thread> threads;
		T fn;
		std::mutex mtx;
		std::condition_variable condition;
		std::atomic<size_t> it;
		bool spin;
};

template<class T, class R, typename... Args> 
Threadpool<T,R,Args...>::Threadpool(const size_t thread_count){
	threads.resize(thread_count);
	this->thread_count = thread_count;
	active_count = 0;
	it = 0;
	spin = false;
}

template<class T, class R, typename... Args> 
Threadpool<T,R,Args...>::Threadpool(const T& fn){
	thread_count = std::thread::hardware_concurrency();
	this->fn = fn;
	threads.resize(thread_count);
	active_count = 0;
	it = 0;
	spin = false;
}

template<class T, class R, typename... Args> 
Threadpool<T,R,Args...>::Threadpool(const size_t thread_count, const T& fn){
	threads.resize(thread_count);
	this->fn = fn;
	this->thread_count = thread_count;
	active_count = 0;
	it = 0;
	spin = false;
}

template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::set_function(const T& fn){
	this->fn = fn;
}

template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::execute_no_atomic(R& input, Args... args){
	active_count = thread_count;
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec, this, std::ref(input), std::ref(it), args...);
	}
}

template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::thread_exec(R& input, std::atomic<size_t>& it, Args... args){
	std::unique_lock<std::mutex> lck(mtx);
	while(it < input.size()) {
		while(spin){
			active_count--;
			condition.wait(lck);
			active_count++;
		}
		size_t currentIndex = it++;
		if(currentIndex >= input.size()) {
			it--;
			break;
		}
		fn(std::ref(input[currentIndex]), args...);
	}
	active_count--;
}

template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::execute_atomic(R& input, Args... args){
	active_count = thread_count;
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec_i, this, std::ref(input), std::ref(it), args...);
	}
}

template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::thread_exec_i(R& input, std::atomic<size_t>& it, Args... args){
	std::unique_lock<std::mutex> lck(mtx);
	while(it < input.size()) {
		while(spin){
			active_count--;
			condition.wait(lck);
			active_count++;
		}
		size_t currentIndex = it++;
		// if(currentIndex >= input.size()) {
		// 	it--;
		// 	break;
		// }
		fn(std::ref(input[currentIndex]), currentIndex, args...);
	}
	active_count--;
}

template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::join(){
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i].join();
	}
}

// sleep and wake not implemented
template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::sleep_all(){
	spin = true;
}

// sleep and wake not implemented
template<class T, class R, typename... Args> 
void Threadpool<T,R,Args...>::wake_all(){
	spin = false;
	condition.notify_all();
}

template<class T, class R, typename... Args> 
size_t Threadpool<T,R,Args...>::get_active_count(){
	return active_count;
}

template<class T, class R, typename... Args> 
size_t Threadpool<T,R,Args...>::get_items_processed(){
	return it;
}

template<class T, class R, typename... Args> 
size_t Threadpool<T,R,Args...>::get_thread_count(){
	return thread_count;
}

#endif