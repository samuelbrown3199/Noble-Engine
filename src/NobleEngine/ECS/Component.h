#pragma once
#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <vector>
#include <memory>

namespace NobleCore
{
	struct SystemBase;

	/**
	*The base struct for the component part of the ECS.
	*/
	struct ComponentBase
	{
		/**
		*Stores the entity ID for the entity this component belongs to.
		*/
		unsigned int entityID;
		/**
		*Stores a weak pointer to the system that handles this component type.
		*/
		static std::weak_ptr<SystemBase> componentSystem;
	};

	/**
	*The parent struct of all component types.
	*/
	template<typename T>
	struct ComponentData : public ComponentBase
	{
		static std::vector<T> componentData;

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
	};
}

#endif