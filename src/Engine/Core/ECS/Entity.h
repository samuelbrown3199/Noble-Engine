#pragma once

#include "../Registry.h"
#include "Behaviour.hpp"
#include "../ToolUI.h"

struct Component;

/**
*Entities act as containers of component data.
*/
struct Entity
{
	/**
	*The unique ID for the entity.
	*/
	std::string m_sEntityID = "";
	/**
	*The name for the entity. This does not necessarily need to be unique.
	*/
	std::string m_sEntityName = "New Entity";

	/**
	* Used to detemine whether the entity can be reused after being deleted.
	*/
	bool m_bAvailableForUse = false;

	std::map<std::string, int> m_vComponents;
	std::vector<Behaviour*> m_vBehaviours;

	std::string m_sEntityParentID = "";
	std::vector<std::string> m_vChildEntityIDs;

	Entity()
	{
		m_sEntityID = "NullEntity";
	}

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

		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();
		for (int i = 0; i < compRegistry->size(); i++)
		{
			int compIndex = compRegistry->at(i).second.m_componentDatalist->GetComponentIndex(m_sEntityID);
			if (compIndex != -1)
				m_vComponents[compRegistry->at(i).first] = compIndex;
		}
	}

	/**
	*Adds a component of the type to the Entity.
	*/
	template<typename T>
	T* AddComponent()
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();

		T comp;
		comp.m_sEntityID = m_sEntityID;
		comp.OnInitialize();
		comp.AddComponent();

		int compIndex = comp.GetComponentIndex(m_sEntityID);
		m_vComponents[comp.GetComponentID()] = compIndex;
		return registry->GetComponent<T>(compIndex);
	}
	/**
	*Adds a component of the type to the Entity.
	*/
	template <typename T, typename ... Args>
	T* AddComponent(Args&&... _args)
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();

		T comp;
		comp.m_sEntityID = m_sEntityID;
		comp.OnInitialize(std::forward<Args>(_args)...);
		comp.AddComponent();

		int compIndex = comp.GetComponentIndex(m_sEntityID);
		m_vComponents[comp.GetComponentID()] = compIndex;
		return registry->GetComponent<T>(compIndex);
	}

	/**
	*Gets a component of the type from the Entity.
	*/
	template<typename T>
	T* GetComponent()
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();
		for (int i = 0; i < m_vComponents.size(); i++)
		{
			T temp;
			T* comp = registry->GetComponent<T>(m_vComponents[temp.GetComponentID()]);
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
		//rework this function I think
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

	void AddBehaviour(Behaviour* beh)
	{
		m_vBehaviours.push_back(beh);
		beh->m_sEntityID = m_sEntityID;
		beh->Start();
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

	std::vector<Behaviour*> GetBehaviours()
	{
		return m_vBehaviours;
	}

	template <typename T>
	void RemoveBehaviour()
	{
		for (int i = 0; i < m_vBehaviours.size(); i++)
		{
			T* beh = dynamic_cast<T*>(m_vBehaviours.at(i));
			if (beh != nullptr)
			{
				m_vBehaviours.erase(m_vBehaviours.begin() + i);
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

	void CreateChildObject()
	{
		Entity* entity = Application::GetApplication()->CreateEntity();
		entity->m_sEntityParentID = m_sEntityID;

		m_vChildEntityIDs.push_back(entity->m_sEntityID);
	}

	void AddChildObject(std::string ID)
	{
		for(int i = 0; i < m_vChildEntityIDs.size(); i++)
			if (m_vChildEntityIDs.at(i) == ID)
				return;

		m_vChildEntityIDs.push_back(ID);
	}

	void RemoveChildObject(std::string ID)
	{
		for (int i = 0; i < m_vChildEntityIDs.size(); i++)
		{
			if (m_vChildEntityIDs.at(i) == ID)
				m_vChildEntityIDs.erase(m_vChildEntityIDs.begin() + i);
		}
	}

	void DoEntityInterface(int& i, int& selEntity, int layer = 0);
	void DoEntityComponentInterface(std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry, std::vector<std::pair<std::string, Behaviour*>>* behaviourRegistry)
	{
		ImGui::SeparatorText("Components");
		ImGui::Indent();
		for (int o = 0; o < compRegistry->size(); o++)
		{
			int compIndex = compRegistry->at(o).second.m_componentDatalist->GetComponentIndex(m_sEntityID);
			Component* comp = compRegistry->at(o).second.m_componentDatalist->GetComponent(compIndex);

			if (comp != nullptr)
			{
				ImGui::SeparatorText(compRegistry->at(o).first.c_str());
				comp->DoComponentInterface();

				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				if (ImGui::Button(FormatString("Remove %s %s", m_sEntityName, compRegistry->at(o).first).c_str()))
				{
					comp->RemoveComponent(m_sEntityID);
				}
				ImGui::Dummy(ImVec2(0.0f, 20.0f));
			}
		}

		ImGui::Unindent();
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("ComponentAdd");

		int selComp = -1;
		if (ImGui::BeginPopup("ComponentAdd"))
		{
			ImGui::SeparatorText("Components");
			for (int i = 0; i < compRegistry->size(); i++)
				if (ImGui::Selectable(compRegistry->at(i).first.c_str()))
					selComp = i;
			ImGui::EndPopup();
		}

		if (selComp != -1)
		{
			compRegistry->at(selComp).second.m_comp->AddComponentToEntity(m_sEntityID);
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::SeparatorText("Behaviours");
		ImGui::Indent();
		for (int o = 0; o < behaviourRegistry->size(); o++)
		{
			Behaviour* beh = behaviourRegistry->at(o).second->GetAsBehaviour(m_sEntityID);

			if (beh != nullptr)
			{
				ImGui::SeparatorText(behaviourRegistry->at(o).first.c_str());
				beh->DoBehaviourInterface();

				ImGui::Dummy(ImVec2(0.0f, 5.0f));
				if (ImGui::Button(FormatString("Remove %s %s", m_sEntityName, behaviourRegistry->at(o).first).c_str()))
				{
					beh->RemoveBehaviourFromEntity(m_sEntityID);
				}
				ImGui::Dummy(ImVec2(0.0f, 20.0f));
			}
		}

		ImGui::Unindent();
		if (ImGui::Button("Add Behaviour"))
			ImGui::OpenPopup("BehaviourAdd");

		int selBeh = -1;
		if (ImGui::BeginPopup("BehaviourAdd"))
		{
			ImGui::SeparatorText("Behaviours");
			for (int i = 0; i < behaviourRegistry->size(); i++)
				if (ImGui::Selectable(behaviourRegistry->at(i).first.c_str()))
					selBeh = i;
			ImGui::EndPopup();
		}

		if (selBeh != -1)
		{
			behaviourRegistry->at(selBeh).second->AddBehaviourToEntity(m_sEntityID);
		}

		ImGui::Dummy(ImVec2(0.0f, 20.0f));
	}
};