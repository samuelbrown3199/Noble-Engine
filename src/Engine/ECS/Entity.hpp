#pragma once

#include "../Core/Registry.h"
#include "Behaviour.hpp"

struct Component;

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

	std::vector<Component*> m_vComponents;
	std::vector<Behaviour*> m_vBehaviours;

	/**
	*Constructor for the Entity.
	*/
	Entity(std::string _ID)
	{
		m_sEntityID = _ID;
	}

	void GetAllComponents()
	{
		m_vComponents.clear();

		std::map<int, std::pair<std::string, ComponentRegistry>>* compRegistry = NobleRegistry::GetComponentRegistry();
		for (int i = 0; i < compRegistry->size(); i++)
		{
			Component* comp = compRegistry->at(i).second.m_comp->GetAsComponent(m_sEntityID);
			if (comp != nullptr)
				m_vComponents.push_back(comp);
		}
	}

	/**
	*Adds a component of the type to the Entity.
	*/
	template<typename T>
	T* AddComponent()
	{
		/*T* alreadyHasComponent = GetComponent<T>();
		if (alreadyHasComponent == nullptr)
		{*/
			T comp;
			comp.m_sEntityID = m_sEntityID;
			comp.OnInitialize();
			comp.AddComponent();

			T* rtnPtr = comp.GetComponent(m_sEntityID);
			m_vComponents.push_back(rtnPtr);
			return rtnPtr;
		//}

		//return alreadyHasComponent;
	}
	/**
	*Adds a component of the type to the Entity.
	*/
	template <typename T, typename ... Args>
	T* AddComponent(Args&&... _args)
	{
		T* alreadyHasComponent = GetComponent<T>();
		if (alreadyHasComponent == nullptr)
		{
			T comp;
			comp.m_sEntityID = m_sEntityID;
			comp.OnInitialize(std::forward<Args>(_args)...);
			comp.AddComponent();

			T* rtnPtr = comp.GetComponent(m_sEntityID);
			m_vComponents.push_back(rtnPtr);
			return rtnPtr;
		}
		return alreadyHasComponent;
	}

	/**
	*Gets a component of the type from the Entity.
	*/
	template<typename T>
	T* GetComponent()
	{
		for (int i = 0; i < m_vComponents.size(); i++)
		{
			T* comp = dynamic_cast<T*>(m_vComponents.at(i));
			if (comp != nullptr)
				return comp;
		}

		return nullptr;
	}

	/**
	* Removes the component from the entity
	*/
	template<typename T>
	void RemoveComponent()
	{
		T::RemoveComponent(m_sEntityID);
		GetAllComponents();
		Application::SetEntitiesDeleted();
	}


	template <typename T, typename ... Args>
	T* AddBehaviour(Args&&... _args)
	{
		T* comp = new T();

		m_vBehaviours.push_back(comp);

		comp->m_sEntityID = m_sEntityID;
		comp->Start();

		return comp;
	}

	template <typename T>
	T* GetBehaviour()
	{
		for (int i = 0; i < m_vBehaviours.size(); i++)
		{
			T* beh = dynamic_cast<T*>(m_vBehaviours.at(i));
			if (beh != nullptr)
				return beh;
		}

		return nullptr;
	}

	template <typename T>
	void RemoveBehaviour()
	{
		for (int i = 0; i < m_vBehaviours.size(); i++)
		{
			T* beh = dynamic_cast<T*>(m_vBehaviours.at(i));
			if (beh != nullptr)
			{
				m_vBehaviours.erase(m_vBehaviour.begin() + i);
				delete beh;

				return;
			}
		}
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