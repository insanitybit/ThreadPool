ThreadPool
==========

This ThreadPool library can take any function and split it across a set number of threads. The implementation is lock-free, and uses atomics
to assign tasks to threads.

Functions:

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
