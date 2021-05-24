#pragma once
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <memory>

//avoid using this in actual engine library compile so intel compiler can be used. Seems like its something to do with some inheritance
#define SetupComponent(T, U) \
std::weak_ptr<SystemBase> U::self; \
std::weak_ptr<SystemBase> T::componentSystem; \
std::vector<T> T::componentData; \

namespace NobleCore
{
	struct SystemBase
	{
		static std::weak_ptr<SystemBase> self;

		virtual void Update() = 0;
		virtual void Render() = 0;

		virtual void AddComponent(int ID) = 0;
	};

	template<typename T>
	class System : public SystemBase
	{
		friend class Application;

	private:

		/**
		*Sets up the system.
		*/
		static void InitializeSystem()
		{
			T::componentSystem = self;
		}
		/**
		*Used to add the component data to the component list.
		*/
		void AddComponent(int ID)
		{
			for (int i = 0; i < Application::entities.size(); i++)
			{
				if (Application::entities.at(i).entityID == ID)
				{
					T comp;
					comp.entityID = ID;
					T::componentData.push_back(comp);
					break;
				}
			}
		}
		/**
		*Loops through the component data and updates them.
		*/
		void Update()
		{
			for (int i = 0; i < T::componentData.size(); i++)
			{
				OnUpdate(&T::componentData.at(i));
			}
		}
		/**
		*Loops through the component data and renders them.
		*/
		void Render()
		{
			for (int i = 0; i < T::componentData.size(); i++)
			{
				OnRender(&T::componentData.at(i));
			}
		}

	protected:
		virtual void OnUpdate(T* comp) {};
		virtual void OnRender(T* comp) {};
	};
}

#endif