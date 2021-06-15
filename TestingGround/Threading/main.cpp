#include <condition_variable>
#include <functional>
#include <vector>
#include <thread>
#include <queue>
#include <iostream>
#include <future>

class ThreadPool
{
public:

	using Task = std::function<void()>;

	explicit ThreadPool(std::size_t numThreads)
	{
		Start(numThreads);
	}

	~ThreadPool()
	{
		Stop();
	}
	template<class T>
	auto Enqueue(T task)->std::future<decltype(task())>
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

private:

	std::vector<std::thread> mThreads;

	std::condition_variable mEventVar;
	std::mutex mEventMutex;
	bool mStopping = false;

	std::queue<Task> mTasks;

	void Start(std::size_t numThreads)
	{
		for (auto i = 0; i < numThreads; ++i)
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
	void Stop() noexcept
	{
		{
			std::unique_lock<std::mutex> lock{ mEventMutex };
			mStopping = true;
		}

		mEventVar.notify_all();
		for (auto& thread : mThreads)
			thread.join();
	}
};

int main()
{
	{
		ThreadPool pool{ 36 };

		auto f1 = pool.Enqueue([] {return 1; });
		auto f2 = pool.Enqueue([] {return 2; });

		std::cout << (f1.get() + f2.get()) << std::endl;
	}
	return 0;
}