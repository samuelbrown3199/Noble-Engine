#include "ThreadingManager.h"

#include "Logger.h"
#include "../Useful.h"

void Thread::ThreadFunction(ThreadingManager* tManager)
{
	while (true)
	{
		Task task;

		{
			std::unique_lock<std::mutex> lock{ tManager->m_EventMutex };

			tManager->m_EventVar.wait(lock, [=] {return tManager->m_bStopping || !tManager->mTasks.empty(); });
			if (tManager->m_bStopping && tManager->mTasks.empty())
				break;

			task = std::move(tManager->mTasks.front());
			tManager->mTasks.pop();
			busy = true;
		}

		task();
		busy = false;
	}
}

ThreadingManager::ThreadingManager()
{
	LogInfo("Initializing thread manager.");
	m_iNumberOfThreads = ceil(std::thread::hardware_concurrency() / 2);
	InitializeThreads();
	LogTrace("Initialized thread manager.");
}

void ThreadingManager::InitializeThreads()
{
	LogTrace(FormatString("Starting %d threads.", m_iNumberOfThreads));

	for (auto i = 0; i < m_iNumberOfThreads; ++i)
	{
		LogTrace(FormatString("Starting thread %d.", i));
		m_vThreads.emplace_back(Thread(this));
	}
}

void ThreadingManager::StopThreads() noexcept
{
	LogInfo("Stopping threads.");
	{
		std::unique_lock<std::mutex> lock{ m_EventMutex };
		m_bStopping = true;
	}

	m_EventVar.notify_all();
	for (int i = 0; i < m_vThreads.size(); i++)
	{
		m_vThreads[i].t.join();
		LogTrace(FormatString("Thread %d stopped.", i));
	}

	LogTrace("Stopped threads.");
}

bool ThreadingManager::AreAllThreadsFinished()
{
	for (int i = 0; i < m_iNumberOfThreads; i++)
	{
		if (!m_vThreads[i].busy)
			return false;
	}

	return true;
}