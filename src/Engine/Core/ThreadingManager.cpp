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
		m_vTaskThreads.emplace_back(Thread(this));
	}
}

void ThreadingManager::StopThreads() noexcept
{
	LogInfo("Stopping threads.");
	{
		std::unique_lock<std::mutex> lock{ m_EventMutex };
		m_bStopping = true;
	}

	std::map<std::string, std::thread>::iterator it;
	for (it = m_mPermanentThreads.begin(); it != m_mPermanentThreads.end(); it++)
	{
		it->second.join();
		LogTrace(FormatString("Thread %s stopped.", it->first.c_str()));
	}
	m_mPermanentThreads.clear();

	m_EventVar.notify_all();
	for (int i = 0; i < m_vTaskThreads.size(); i++)
	{
		m_vTaskThreads[i].t.join();
		LogTrace(FormatString("Thread %d stopped.", i));
	}

	LogTrace("Stopped threads.");
}

bool ThreadingManager::AreAllThreadsFinished()
{
	for (int i = 0; i < m_iNumberOfThreads; i++)
	{
		if (!m_vTaskThreads[i].busy)
			return false;
	}

	return true;
}

void ThreadingManager::AddPermanentThread(std::string ID, std::thread& thread)
{
	if (m_mPermanentThreads.count(ID) == 0)
	{
		m_mPermanentThreads[ID] = std::move(thread);
		LogInfo(FormatString("Added permanent thread %s.", ID.c_str()));
	}
	else
		LogFatalError(FormatString("Trying to add permanent thread %s that already exists.", ID.c_str()));
}