#pragma once
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <memory>

#include "../Core/ThreadingManager.h"

//avoid using this in actual engine library compile so intel compiler can be used. Seems like its something to do with some inheritance
#define SetupComponent(T, U) \
std::weak_ptr<SystemBase> U::self; \
std::weak_ptr<SystemBase> T::componentSystem; \
std::vector<T> T::componentData; \

namespace NobleCore
{
	struct SystemBase
	{
		bool useUpdate = true, useRender = true, useThreads = false;
		/**
		*Determines how many components a thread should handle.
		*/
		int maxComponentsPerThread = 1024;

		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void RemoveComponent(unsigned int _ID) = 0;
	};

	template<typename T>
	/**
	* Inherit from this to create new systems.
	*/
	class System : public SystemBase
	{
		friend class Application;

	private:
		/**
		*Keeps a weak pointer to itself.
		*/
		static std::weak_ptr<SystemBase> self;
		/**
		*Sets up the system.
		*/
		static void InitializeSystem()
		{
			T::componentSystem = self;
		}
		/**
		*Loops through the component data and updates them.
		*/
		void Update()
		{
			if (useUpdate)
			{
				if (!useThreads)
				{
					for (int i = 0; i < T::componentData.size(); i++)
					{
						OnUpdate(&T::componentData.at(i));
					}
				}
				else
				{
					int amountOfThreads = ceil(T::componentData.size() / maxComponentsPerThread) + 1;
					for (int i = 0; i < amountOfThreads; i++)
					{
						int buffer = maxComponentsPerThread * i;
						auto th = ThreadingManager::EnqueueTask([&] { ThreadUpdate(buffer, maxComponentsPerThread); });
					}
				}
			}
		}
		void ThreadUpdate(int _buffer, int _amount)
		{
			int maxCap = _buffer + _amount;
			for (size_t co = _buffer; co < maxCap; co++)
			{
				if (co >= T::componentData.size())
					return;
				
				OnUpdate(&T::componentData.at(co));
			}
		}
		/**
		*Loops through the component data and renders them.
		*/
		void Render()
		{
			if (useRender)
			{
				for (int i = 0; i < T::componentData.size(); i++)
				{
					OnRender(&T::componentData.at(i));
				}
			}
		}

		void RemoveComponent(unsigned int _ID)
		{
			for (int i = 0; i < T::componentData.size(); i++)
			{
				if (T::componentData.at(i).entityID == _ID)
				{
					T::componentData.erase(T::componentData.begin() + i);
					break;
				}
			}
		}

	protected:
		/**
		* Overwrite this to create functionality on a component every update.
		*/
		virtual void OnUpdate(T* comp) {};
		/**
		* Overwrite this to create functionality on a component every rendering process.
		*/
		virtual void OnRender(T* comp) {};
	};
}

#endif