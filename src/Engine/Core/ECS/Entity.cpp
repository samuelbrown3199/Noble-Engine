#include "Entity.h"

#include "../CommandTypes.h"

void Entity::DoEntityInterface(int& i, int& selEntity, int layer)
{
	if (m_bAvailableForUse)
		return;

	//Child Entity, time to skip
	if (m_sEntityParentID != "" && layer == 0)
		return;

	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
	// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
	ImGuiTreeNodeFlags node_flags = base_flags;

	bool is_selected = false;
	Entity* pSelEntity = Application::GetApplication()->GetEntity(selEntity);
	if (pSelEntity != nullptr)
		is_selected = pSelEntity->m_sEntityID == m_sEntityID;
	if (is_selected)
		node_flags |= ImGuiTreeNodeFlags_Selected;

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, m_sEntityName.c_str());
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		selEntity = Application::GetApplication()->GetEntityIndex(m_sEntityID);

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Create Child Entity"))
		{
			AddEntityCommand* command = new AddEntityCommand("New Child Entity", m_sEntityID);
			Application::GetApplication()->PushCommand(command);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			DeleteEntityCommand* command = new DeleteEntityCommand(m_sEntityID);
			Application::GetApplication()->PushCommand(command);

			ImGui::CloseCurrentPopup();
		}

		if (ImGui::BeginMenu("Add Component"))
		{
			std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();
			for (int o = 0; o < compRegistry->size(); o++)
			{
				if (ImGui::MenuItem(compRegistry->at(o).first.c_str()))
				{
					compRegistry->at(o).second.m_comp->AddComponentToEntity(m_sEntityID);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	if (node_open)
	{
		if (ImGui::InputText("Entity Name", &m_sEntityName, 128))
		{
			ChangeValueCommand<std::string>* command = new ChangeValueCommand<std::string>(&m_sEntityName, m_sEntityName);
			Application::GetApplication()->PushCommand(command);
		}
		ImGui::Text(FormatString("Entity ID: %s", m_sEntityID.c_str()).c_str());

		if (m_vChildEntityIDs.size() != 0)
		{
			ImGui::Text("Child Entities");
			//List all child entities.
			for (int o = 0; o < m_vChildEntityIDs.size(); o++)
			{
				int childIndex = Application::GetApplication()->GetEntityIndex(m_vChildEntityIDs.at(o));
				Application::GetApplication()->GetEntity(childIndex)->DoEntityInterface(i, selEntity, layer + 1);
			}
		}

		ImGui::TreePop();
	}
}