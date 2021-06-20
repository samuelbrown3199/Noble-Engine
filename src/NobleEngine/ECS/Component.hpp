#pragma once
#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <vector>
#include <memory>

namespace NobleCore
{
	struct SystemBase;

	/**
	*The parent struct of all component types. Create your own component types by inheriting from this, for example struct Transform : public ComponentData<Transform>.
	*/
	template<typename T>
	struct ComponentData
	{
		friend struct Entity;
		/**
		*Stores the full list of the component types data.
		*/
		static std::vector<T> componentData;
		/**
		*Stores the entity ID for the entity this component belongs to.
		*/
		unsigned int entityID;
		/**
		*Stores a weak pointer to the system that handles this component type.
		*/
		static std::weak_ptr<SystemBase> componentSystem;

		virtual void OnInitialize() {};

	private:

		static T* GetComponent(int _ID)
		{
			for (int i = 0; i < componentData.size(); i++)
			{
				if (componentData.at(i).entityID == _ID)
				{
					return &T::componentData.at(i);
				}
			}

			return nullptr;
		}

		static void RemoveComponent(int _ID)
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
	};
}

#endif