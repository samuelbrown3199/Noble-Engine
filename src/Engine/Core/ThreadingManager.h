#pragma once
#ifndef THREADINGMANAGER_H_
#define THREADINGMANAGER_H_

#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <iostream>
#include <future>

using Task = std::function<void()>;

class ThreadingManager;

struct Thread
{
	std::thread t;
	bool busy = false;

	Thread(ThreadingManager* tManager)
	{
		t = std::thread(&Thread::ThreadFunction, this, tManager);
	}

	void ThreadFunction(ThreadingManager* tManager);
};

/**
* Responsible for handling threads.
*/
class ThreadingManager
{
	friend class Application;
	friend struct Thread;
public:

	ThreadingManager();

	template<class T>
	/**
	* Adds a task to the task list. A thread then picks up the task and calls the function.
	*/
	auto EnqueueTask(T task)->std::future<decltype(task())>
	{
		auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
		{
			std::unique_lock<std::mutex> lock{ m_EventMutex };
			mTasks.emplace([=]
				{
					(*wrapper)();
				}
			);
		}

		m_EventVar.notify_one();
		return wrapper->get_future();
	}

	void WaitForTasksToClear()
	{
		while (!mTasks.empty() /* && !AreAllThreadsFinished()*/) {}
	}

private:

	int m_iNumberOfThreads;
	std::vector<Thread> m_vThreads;

	std::condition_variable m_EventVar;
	std::mutex m_EventMutex;
	bool m_bStopping;

	std::queue<Task> mTasks;

	void InitializeThreads();
	void StopThreads() noexcept;
	bool AreAllThreadsFinished();
};

#endif