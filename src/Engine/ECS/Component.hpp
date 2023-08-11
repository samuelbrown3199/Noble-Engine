#pragma once

#include <vector>
#include <memory>

#include <nlohmann/json.hpp>

#include "../imgui/imgui.h"

struct SystemBase;

struct Component
{
	/**
	*Stores the entity ID for the entity this component belongs to.
	*/
	std::string m_sEntityID;

	virtual void DoComponentInterface()
	{
		ImGui::Text(m_sEntityID.c_str());
		ImGui::Text("No editable values. Component window not defined for this type.");
	}
};

/**
*The parent struct of all component types. Create your own component types by inheriting from this, for example struct Transform : public ComponentData<Transform>.
*/
template<typename T>
struct ComponentData : public Component
{
	friend struct Entity;
	/**
	*Stores the full list of the component types data.
	*/
	static std::vector<T> componentData;
	/**
	*Stores a weak pointer to the system that handles this component type.
	*/
	static std::weak_ptr<SystemBase> componentSystem;

	/**
	* This can be used to create initialisation functions for components.
	*/
	virtual void OnInitialize(){};
	/**
	* This is used when deleting component data. Useful for cleaning up memory etc.
	*/
	virtual void OnRemove() {};

	virtual nlohmann::json WriteJson() { nlohmann::json data; return data; };
	virtual void FromJson(const nlohmann::json& j) {};

	static int GetComponentIndex(std::string _ID)
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			if (componentData.at(i).m_sEntityID == _ID)
			{
				return i;
			}
		}

		return -1;
	}

	static T* GetComponent(std::string _ID)
	{
		for (int i = 0; i < componentData.size(); i++)
		{
			if (componentData.at(i).m_sEntityID == _ID)
			{
				return &T::componentData.at(i);
			}
		}

		return nullptr;
	}

private:

	static void RemoveComponent(std::string _ID)
	{
		for (int i = 0; i < T::componentData.size(); i++)
		{
			if (T::componentData.at(i).m_sEntityID == _ID)
			{
				T::componentData.at(i)->OnRemove();
				T::componentData.erase(T::componentData.begin() + i);
				break;
			}
		}
	}
};