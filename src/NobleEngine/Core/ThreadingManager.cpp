#include "ThreadingManager.h"

namespace NobleCore
{
	int ThreadingManager::numberOfThreads;
	std::vector<std::thread> ThreadingManager::mThreads;

	std::condition_variable ThreadingManager::mEventVar;
	std::mutex ThreadingManager::mEventMutex;
	bool ThreadingManager::mStopping = false;

	std::queue<Task> ThreadingManager::mTasks;

	ThreadingManager::ThreadingManager()
	{
		numberOfThreads = std::thread::hardware_concurrency() / 2;
		InitializeThreads();
	}
	ThreadingManager::~ThreadingManager()
	{
		StopThreads();
	}

	void ThreadingManager::InitializeThreads()
	{
		for (auto i = 0; i < numberOfThreads; ++i)
		{
			mThreads.emplace_back([=]
				{
					while (true)
					{
						Task task;

						{
							std::unique_lock<std::mutex> lock{ mEventMutex };

							mEventVar.wait(lock, [=] {return mStopping || !mTasks.empty(); });
							if (mStopping && mTasks.empty())
								break;

							task = std::move(mTasks.front());
							mTasks.pop();
						}

						task();
					}
				});
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
			thread.join();
	}

	bool ThreadingManager::AreAllThreadsFinished()
	{
		for (int i = 0; i < numberOfThreads; i++)
		{
			if (!mThreads[i].joinable())
				return false;
		}

		return true;
	}
}