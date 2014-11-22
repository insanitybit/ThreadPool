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
using namespace std;
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
		~Threadpool();

	private:
		void thread_exec();
		void thread_exec_i();
		size_t thread_count;
		size_t active_count;
		vector<thread> threads;
		R input;
		S output;
		T fn;
		mutex mtx;
		bool t;
	
};

template<class T, class R, class S> Threadpool<T,R,S>::Threadpool(size_t count){
	threads.resize(count);
	thread_count = count;
	active_count = 0;
}

template<class T, class R, class S> void Threadpool<T,R,S>::set_function(T f){
	fn = f;
}

template<class T, class R, class S> void Threadpool<T,R,S>::set_input(R& i){
	input = i;
}

template<class T, class R, class S> void Threadpool<T,R,S>::set_output(S& o){
	output = o;
}

template<class T, class R, class S> void Threadpool<T,R,S>::execute_no_atomic(){
	for (int i = 0; i < thread_count; ++i)
	{
		threads[i] = thread(&Threadpool::thread_exec, this);
	}
}

template<class T, class R, class S> void Threadpool<T,R,S>::thread_exec(){
size_t currentIndex = 0;
atomic<size_t> it;

	while(currentIndex < input.size()) {
		mtx.lock();
	   
	    if(currentIndex >= input.size()) {
	        break;
	    }
	   
		it = currentIndex;
		currentIndex++;
		mtx.unlock();

		fn(ref(input[it]), ref(output));
	}
}

template<class T, class R, class S> void Threadpool<T,R,S>::execute_atomic(){
	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i] = thread(&Threadpool::thread_exec_i, this);
	}
}

template<class T, class R, class S> void Threadpool<T,R,S>::thread_exec_i(){
size_t currentIndex = 0;
atomic<size_t> it;

	while(currentIndex < input.size()) {
		mtx.lock();
	   
	    if(currentIndex >= input.size()) {
	        break;
	    }
	   
		it = currentIndex;
		currentIndex++;
		mtx.unlock();

		fn(ref(input[it]), ref(output), it);
	}
}

template<class T, class R, class S> void Threadpool<T,R,S>::join(){

	for (size_t i = 0; i < thread_count; ++i)
	{
		threads[i].join();
	}
}

template<class T, class R, class S> S Threadpool<T,R,S>::get_output(){

	return output;
}

template<class T, class R, class S> Threadpool<T,R,S>::~Threadpool(){
	threads.clear();
}
#endif