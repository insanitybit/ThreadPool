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

template<class Function_T, typename... Args>
class Threadpool
{
	public:
		Threadpool(const size_t, const Function_T&);

		void set_function(const Function_T&);
		void set_thread_count(const size_t);

		void execute_no_atomic(const size_t, Args&...);

		void execute_atomic(const size_t, Args&...);

		void join();
		void sleep_all();
		void wake_all();

		size_t get_active_count();
		size_t get_items_processed();
		size_t get_thread_count();

		void detach();
		void reset();

	private:
		void thread_exec(const size_t, Args&...);
		void thread_exec_i(const size_t, Args&...);
		size_t thread_count;
		size_t active_count;
		std::vector<std::thread> threads;
		Function_T fn;
		std::mutex mtx;
		std::condition_variable condition;
		std::atomic<size_t> it;
		bool spin;
};


template<class Function_T>
class Threadpool<Function_T>
{
	public:
		Threadpool(const size_t, const Function_T&);

		void set_function(const Function_T&);
		void set_thread_count(const size_t);

		void execute_no_atomic(const size_t);

		void execute_atomic(const size_t);

		void join();
		void sleep_all();
		void wake_all();

		size_t get_active_count();
		size_t get_items_processed();
		size_t get_thread_count();

		void detach();
		void reset();

	private:
		void thread_exec(const size_t);
		void thread_exec_i(const size_t);
		size_t thread_count;
		size_t active_count;
		std::vector<std::thread> threads;
		Function_T fn;
		std::mutex mtx;
		std::condition_variable condition;
		std::atomic<size_t> it;
		bool spin;
};



template<class Function_T, typename... Args> 
Threadpool<Function_T, Args...>::Threadpool(const size_t thread_count, const Function_T& fn){
	threads.resize(thread_count);
	this->fn = fn;
	this->thread_count = thread_count;
	active_count = 0;
	it = 0;
	spin = false;
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::set_function(const Function_T& fn){
	this->fn = fn;
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::execute_no_atomic(const size_t count, Args&... args){
	active_count = thread_count;
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec, this, count, std::ref(args)...);
	}
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::thread_exec(const size_t count, Args&... args){
	std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
	while(it < count) {
		while(spin){
			active_count--;
			condition.wait(lck);
			active_count++;
		}
		size_t currentIndex = it++;
		if(currentIndex >= count) {
			it--;
			break;
		}
		fn(std::ref(args)...);
	}
	active_count--;
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::execute_atomic(const size_t count, Args&... args){
	active_count = thread_count;
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec_i, this, count, std::ref(args)...);
	}
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::thread_exec_i(const size_t count, Args&... args){
	std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
	while(it < count) {
		while(spin){
			active_count--;
			condition.wait(lck);
			active_count++;
		}
		size_t currentIndex = it++;
		if(currentIndex >= count) {
			it--;
			break;
		}
		fn(currentIndex, std::ref(args)...);
	}
	active_count--;
}


template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::join(){
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i].join();
	}
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::sleep_all(){
	spin = true;
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::wake_all(){
	spin = false;
	condition.notify_all();
}

template<class Function_T, typename... Args> 
size_t Threadpool<Function_T, Args...>::get_active_count(){
	return active_count;
}

template<class Function_T, typename... Args> 
size_t Threadpool<Function_T, Args...>::get_items_processed(){
	return it;
}

template<class Function_T, typename... Args> 
size_t Threadpool<Function_T, Args...>::get_thread_count(){
	return thread_count;
}


template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::set_thread_count(const size_t s){
	thread_count = s;
	threads.resize(thread_count);
}


template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::reset(){
	it = 0;
	spin = false;
	threads.clear();
}

template<class Function_T, typename... Args> 
void Threadpool<Function_T, Args...>::detach(){
	for (size_t i = 0; i < threads.size(); ++i)
	{
		threads[i].detach();
	}
}



/*

	For functions that take no arguments

*/


template<class Function_T> 
Threadpool<Function_T>::Threadpool(const size_t thread_count, const Function_T& fn){
	threads.resize(thread_count);
	this->fn = fn;
	this->thread_count = thread_count;
	active_count = 0;
	it = 0;
	spin = false;
}

template<class Function_T> 
void Threadpool<Function_T>::set_function(const Function_T& fn){
	this->fn = fn;
}

template<class Function_T> 
void Threadpool<Function_T>::execute_no_atomic(const size_t count){
	active_count = thread_count;
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec, this, count);
	}
}

template<class Function_T> 
void Threadpool<Function_T>::thread_exec(const size_t count){
	std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
	while(it < count) {
		while(spin){
			active_count--;
			condition.wait(lck);
			active_count++;
		}
		size_t currentIndex = it++;
		if(currentIndex >= count) {
			it--;
			break;
		}
		fn();
	}
	active_count--;
}

template<class Function_T> 
void Threadpool<Function_T>::execute_atomic(const size_t count){
	active_count = thread_count;
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = std::thread(&Threadpool::thread_exec_i, this, count);
	}
}

template<class Function_T> 
void Threadpool<Function_T>::thread_exec_i(const size_t count){
	std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
	while(it < count) {
		while(spin){
			active_count--;
			condition.wait(lck);
			active_count++;
		}
		size_t currentIndex = it++;
		if(currentIndex >= count) {
			it--;
			break;
		}
		fn(currentIndex);
	}
	active_count--;
}


template<class Function_T> 
void Threadpool<Function_T>::join(){
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i].join();
	}
}

template<class Function_T> 
void Threadpool<Function_T>::sleep_all(){
	spin = true;
}

template<class Function_T> 
void Threadpool<Function_T>::wake_all(){
	spin = false;
	condition.notify_all();
}

template<class Function_T> 
size_t Threadpool<Function_T>::get_active_count(){
	return active_count;
}

template<class Function_T> 
size_t Threadpool<Function_T>::get_items_processed(){
	return it;
}

template<class Function_T> 
size_t Threadpool<Function_T>::get_thread_count(){
	return thread_count;
}


template<class Function_T> 
void Threadpool<Function_T>::set_thread_count(const size_t s){
	thread_count = s;
	threads.resize(thread_count);
}


template<class Function_T> 
void Threadpool<Function_T>::reset(){
	it = 0;
	spin = false;
	threads.clear();
}

template<class Function_T> 
void Threadpool<Function_T>::detach(){
	for (size_t i = 0; i < threads.size(); ++i)
	{
		threads[i].detach();
	}

}


#endif