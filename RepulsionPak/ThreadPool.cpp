
#include "ThreadPool.h"

//ThreadPool::ThreadPool(unsigned int n)
//	: busy()
//	, processed()
//	, stop()
//{
//	//std::cout << "num_thread = " << n << "\n";
//
//	for (unsigned int i = 0; i < n; ++i)
//		workers.emplace_back(std::bind(&ThreadPool::thread_proc, this));
//}
//
//ThreadPool::~ThreadPool()
//{
//	// set stop-condition
//	std::unique_lock<std::mutex> latch(queue_mutex);
//	stop = true;
//	cv_task.notify_all();
//	latch.unlock();
//
//	// all threads terminate, then we're done.
//	for (auto& t : workers)
//		t.join();
//}
//
//void ThreadPool::thread_proc()
//{
//	while (true)
//	{
//		std::unique_lock<std::mutex> latch(queue_mutex);
//		cv_task.wait(latch, [this]() { return stop || !tasks.empty(); });
//		if (!tasks.empty())
//		{
//			// got work. set busy.
//			++busy;
//
//			// pull from queue
//			auto fn = tasks.front();
//			tasks.pop_front();
//
//			// release lock. run async
//			latch.unlock();
//
//			// run function outside context
//			fn();
//			++processed;
//
//			latch.lock();
//			--busy;
//			cv_finished.notify_one();
//		}
//		else if (stop)
//		{
//			break;
//		}
//	}
//}
//
////// https_//github.com/mtrebi/thread-pool
////template<typename F, typename...Args>
////void ThreadPool::submit(F&& f, Args&&... args)
////{
////	// Create a function with bounded parameters ready to execute
////	std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
////	// Encapsulate it into a shared ptr in order to be able to copy construct / assign 
////	auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);
////
////	// Wrap packaged task into void function
////	std::function<void()> wrapper_func = [task_ptr]() {
////		(*task_ptr)();
////	};
////
////	// ROFLMAO
////	enqueue(wrapper_func);
////}
//
//// generic function push
//template<class F>
//void ThreadPool::enqueue(F&& f)
//{
//	std::unique_lock<std::mutex> lock(queue_mutex);
//	tasks.emplace_back(std::forward<F>(f));
//	cv_task.notify_one();
//}
//
//// waits until the queue is empty.
//void ThreadPool::waitFinished()
//{
//	std::unique_lock<std::mutex> lock(queue_mutex);
//	cv_finished.wait(lock, [this]() { return tasks.empty() && (busy == 0); });
//}