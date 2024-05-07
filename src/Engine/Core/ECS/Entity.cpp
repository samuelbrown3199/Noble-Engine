#include "Entity.h"

#include "../CommandTypes.h"

void Entity::DoEntityInterface(int& i, bool& node_open, std::string& selEntity, int layer)
{
	if (m_bAvailableForUse)
		return;

	//Child Entity, time to skip
	if (m_sEntityParentID != "" && layer == 0)
		return;

	if (m_pNameEdit == nullptr)
	{
		m_pNameEdit = new NobleTextInput();
		m_pNameEdit->m_sID = m_sEntityID;
		m_pNameEdit->m_pEntity = this;

		m_pNameEdit->Initialize(&m_sEntityName);
	}

	if (node_open)
	{
		m_pNameEdit->DoTextInput("Entity Name", m_bInitializeInterface, &m_sEntityName);
		ImGui::SetItemTooltip("Change the name of the entity.");

		ImGui::Text(FormatString("Entity ID: %s", m_sEntityID.c_str()).c_str());
		ImGui::SetItemTooltip("The unique identifier for the entity.");

		if (m_vChildEntityIDs.size() != 0)
		{
			ImGui::Text("Child Entities");

			//List all child entities.
			for (int o = 0; o < m_vChildEntityIDs.size(); o++)
			{
				static EntityDropdown entityDropdown;
				entityDropdown.DoEntityDropdown(m_vChildEntityIDs.at(o), o, selEntity, layer + 1);
			}
		}

		ImGui::TreePop();
	}

	m_bInitializeInterface = false;
}

void Entity::DoEntityComponentInterface(std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry)
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
			if (ImGui::BeginPopupContextItem(compRegistry->at(o).first.c_str()))
			{
				if (ImGui::Button("Remove"))
				{
					RemoveComponentCommand* command = new RemoveComponentCommand(m_sEntityID, o);
					Application::GetApplication()->PushCommand(command);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			comp->DoComponentInterface();

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
			if (ImGui::Button(FormatString("Remove %s", compRegistry->at(o).first).c_str()))
			{
				RemoveComponentCommand* command = new RemoveComponentCommand(m_sEntityID, o);
				Application::GetApplication()->PushCommand(command);
			}
			ImGui::Dummy(ImVec2(0.0f, 20.0f));
		}
	}

	ImGui::Unindent();
	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("ComponentAdd");

	if (ImGui::BeginPopup("ComponentAdd"))
	{
		ImGui::SeparatorText("Components");
		for (int i = 0; i < compRegistry->size(); i++)
		{
			if (ImGui::Selectable(compRegistry->at(i).first.c_str()))
			{
				AddComponentCommand* command = new AddComponentCommand(m_sEntityID, i);
				Application::GetApplication()->PushCommand(command);
			}
		}
		ImGui::EndPopup();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
}