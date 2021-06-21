#include "ThreadingManager.h"

namespace NobleCore
{
	int ThreadingManager::numberOfThreads;
	std::vector<NobleThread> ThreadingManager::mThreads;

	std::condition_variable ThreadingManager::mEventVar;
	std::mutex ThreadingManager::mEventMutex;
	bool ThreadingManager::mStopping = false;

	std::queue<Task> ThreadingManager::mTasks;

	void NobleThread::ThreadFunction()
	{
		while (true)
		{
			Task task;

			{
				std::unique_lock<std::mutex> lock{ ThreadingManager::mEventMutex };

				ThreadingManager::mEventVar.wait(lock, [=] {return ThreadingManager::mStopping || !ThreadingManager::mTasks.empty(); });
				if (ThreadingManager::mStopping && ThreadingManager::mTasks.empty())
					break;

				task = std::move(ThreadingManager::mTasks.front());
				ThreadingManager::mTasks.pop();
				busy = true;
			}

			task();
			busy = false;
		}
	}

	ThreadingManager::ThreadingManager()
	{
		numberOfThreads = ceil(std::thread::hardware_concurrency() / 2);
		InitializeThreads();
	}

	void ThreadingManager::InitializeThreads()
	{
		for (auto i = 0; i < numberOfThreads; ++i)
		{
			mThreads.emplace_back();
		}
	}

	void ThreadingManager::StopThreads() noexcept
	{
		{
			std::unique_lock<std::mutex> lock{ mEventMutex };
			mStopping = true;
		}

		mEventVar.notify_all();
		for (auto& thread : mThreads)
			thread.t.join();
	}

	bool ThreadingManager::AreAllThreadsFinished()
	{
		for (int i = 0; i < numberOfThreads; i++)
		{
			if (!mThreads[i].busy)
				return false;
		}

		return true;
	}
}