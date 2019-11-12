#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <iostream>
#include <deque>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <random>
#include <atomic>
#include <future>

// thanks bruh
// https_//stackoverflow.com/questions/23896421/efficiently-waiting-for-all-tasks-in-a-threadpool-to-finish

class ThreadPool
{
private:
	std::vector<std::thread>          _workers;
	std::deque<std::function<void()>> _tasks;
	std::mutex                        _queue_mutex;
	std::condition_variable           _cv_task;
	std::condition_variable           _cv_finished;
	std::atomic_uint                  _processed;
	unsigned int                      _busy;
	bool                              _stop;

public:

	ThreadPool(unsigned int n = std::thread::hardware_concurrency()) : _busy(), _processed(), _stop()
	{
		for (unsigned int i = 0; i < n; ++i)
		{
			_workers.emplace_back(std::bind(&ThreadPool::thread_proc, this)); // std::vector<std::thread>
		}
	}

	~ThreadPool()
	{
		// set stop-condition
		std::unique_lock<std::mutex> latch(_queue_mutex);
		_stop = true;
		_cv_task.notify_all();
		latch.unlock();

		// all threads terminate, then we're done.
		for (auto& t : _workers)
			t.join();
	}

	
	// https_//github.com/mtrebi/thread-pool
	template<typename F, typename...Args>
	void submit(F&& f, Args&&... args)
	{
		// Create a function with bounded parameters ready to execute
		//std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
		//std::function<decltype(f(args...))()> func = std::bind(f, args...);
		auto g = std::bind(f, args...);
		// Encapsulate it into a shared ptr in order to be able to copy construct / assign 
		//auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

		// Wrap packaged task into void function
		/*std::function<void()> wrapper_func = [task_ptr]() {
			(*task_ptr)();
		};*/
		
		//enqueue(wrapper_func); // ROFLMAO
		enqueue(g); // ROFLMAO
	}

	// generic function push
	template<class F>
	void enqueue(F&& f)
	{
		std::unique_lock<std::mutex> lock(_queue_mutex);
		_tasks.emplace_back(std::forward<F>(f));
		_cv_task.notify_one();
	}

	// waits until the queue is empty.
	void waitFinished()
	{
		std::unique_lock<std::mutex> lock(_queue_mutex);
		_cv_finished.wait(lock, [this]() { return _tasks.empty() && (_busy == 0); });
	}

private:

	void thread_proc()
	{
		while (true)
		{
			std::unique_lock<std::mutex> latch(_queue_mutex);
			_cv_task.wait(latch, [this]() { return _stop || !_tasks.empty(); });
			if (!_tasks.empty())
			{
				// got work. set busy.
				++_busy;

				// pull from queue
				auto fn = _tasks.front();
				_tasks.pop_front();

				// release lock. run async
				latch.unlock();

				// run function outside context
				fn();
				++_processed;

				latch.lock();
				--_busy;
				_cv_finished.notify_one();
			}
			else if (_stop)
			{
				break;
			}
		}
	}
};


#endif