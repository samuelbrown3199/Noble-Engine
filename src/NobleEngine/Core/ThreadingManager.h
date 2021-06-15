#ifndef THREADINGMANAGER_H_
#define THREADINGMANAGER_H_

#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <iostream>
#include <future>

namespace NobleCore
{
	using Task = std::function<void()>;
	/**
	* Responsible for handling threads.
	*/
	class ThreadingManager
	{

	public:

		ThreadingManager();
		~ThreadingManager();

		template<class T>
		/**
		* Adds a task to the task list. A thread then picks up the task and calls the function.
		*/
		static auto EnqueueTask(T task)->std::future<decltype(task())>
		{
			auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));
			{
				std::unique_lock<std::mutex> lock{ mEventMutex };
				mTasks.emplace([=]
					{
						(*wrapper)();
					}
				);
			}

			mEventVar.notify_one();
			return wrapper->get_future();
		}

		static void WaitForTasksToClear()
		{
			while (!mTasks.empty() && AreAllThreadsFinished()) {}
		}

	private:

		static int numberOfThreads;
		static std::vector<std::thread> mThreads;

		static std::condition_variable mEventVar;
		static std::mutex mEventMutex;
		static bool mStopping;

		static std::queue<Task> mTasks;

		void InitializeThreads();
		void StopThreads() noexcept;
		static bool AreAllThreadsFinished();
	};
}

#endif