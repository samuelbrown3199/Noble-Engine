#pragma once
#ifndef ENTITY_H_
#define ENTITY_H_

#include "Behaviour.hpp"

/**
*Entities act as containers of component data.
*/
struct Entity
{
	/**
	*The unique ID for the entity.
	*/
	std::string m_sEntityID;
	/**
	*The name for the entity. This does not necessarily need to be unique.
	*/
	std::string m_sEntityName;

	/**
	* Used to detemine whether the entity can be reused after being deleted.
	*/
	bool m_bAvailableForUse = false;

	std::vector<Behaviour*> m_vBehaviours;

	/**
	*Constructor for the Entity.
	*/
	Entity(std::string _ID)
	{
		m_sEntityID = _ID;
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
			comp.m_sEntityID = m_sEntityID;
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
			comp.m_sEntityID = m_sEntityID;
			comp.OnInitialize(std::forward<Args>(_args)...);
			T::componentData.push_back(comp);

			return &T::componentData.at(T::componentData.size() - 1);
		}
		return alreadyHasComponent;
	}

	template <typename T, typename ... Args>
	T* AddBehaviour(Args&&... _args)
	{
		T* comp = new T();
		comp->m_sEntityID = m_sEntityID;
		comp->Start();

		m_vBehaviours.push_back(comp);
		return comp;
	}

	/**
	*Gets a component of the type from the Entity.
	*/
	template<typename T>
	T* GetComponent()
	{
		T* temp = T::GetComponent(m_sEntityID);
		return temp;
	}

	/**
	* Removes the component from the entity
	*/
	template<typename T>
	void RemoveComponent()
	{
		T::RemoveComponent(m_sEntityID);
	}

	void DeleteAllBehaviours()
	{
		for (int i = 0; i < m_vBehaviours.size(); i++)
		{
			delete m_vBehaviours.at(i);
		}

		m_vBehaviours.clear();
	}
};

#endif