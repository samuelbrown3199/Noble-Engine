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
					AddComponentCommand* command = new AddComponentCommand(m_sEntityID, o);
					Application::GetApplication()->PushCommand(command);
				}
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Add Behaviour"))
		{
			std::vector<std::pair<std::string, Behaviour*>>* behaviourRegistry = Application::GetApplication()->GetRegistry()->GetBehaviourRegistry();
			for (int o = 0; o < behaviourRegistry->size(); o++)
			{
				if (ImGui::MenuItem(behaviourRegistry->at(o).first.c_str()))
				{
					behaviourRegistry->at(o).second->AddBehaviourToEntity(m_sEntityID);
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

void Entity::DoEntityComponentInterface(std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry, std::vector<std::pair<std::string, Behaviour*>>* behaviourRegistry)
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