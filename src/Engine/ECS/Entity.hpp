#pragma once
#ifndef ENTITY_H_
#define ENTITY_H_

/**
*Entities act as containers of component data.
*/
struct Entity
{
	/**
	*The unique ID for the entity.
	*/
	unsigned int entityID;
	/**
	* Used to detemine whether the entity can be reused after being deleted.
	*/
	bool availableForUse = false;

	/**
	*Constructor for the Entity.
	*/
	Entity(unsigned int _ID)
	{
		entityID = _ID;
	}

	/**
	*Adds a component of the type to the Entity.
	*/
	template<typename T>
	T* AddComponent()
	{
		T* alreadyHasComponent = GetComponent<T>();
		if (!alreadyHasComponent)
		{
			T comp;
			comp.entityID = entityID;
			comp.OnInitialize();
			T::componentData.push_back(comp);

			return &T::componentData.at(T::componentData.size() - 1);
		}
		return alreadyHasComponent;
	}
	/**
	*Adds a component of the type to the Entity.
	*/
	template <typename T, typename ... Args>
	T* AddComponent(Args&&... _args)
	{
		T* alreadyHasComponent = GetComponent<T>();
		if (!alreadyHasComponent)
		{
			T comp;
			comp.entityID = entityID;
			comp.OnInitialize(std::forward<Args>(_args)...);
			T::componentData.push_back(comp);

			return &T::componentData.at(T::componentData.size() - 1);
		}
		return alreadyHasComponent;
	}
	/**
	*Gets a component of the type from the Entity.
	*/
	template<typename T>
	T* GetComponent()
	{
		T* temp = T::GetComponent(entityID);
		return temp;
	}
	template<typename T>
	/**
	* Removes the component from the entity
	*/
	void RemoveComponent()
	{
		T::RemoveComponent(entityID);
	}
};

#endif