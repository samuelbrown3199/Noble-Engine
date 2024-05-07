#include "ToolUI.h"

#include "Application.h"
#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "CommandTypes.h"

#include "ResourceManager.h"

void NobleColourEdit::DoColourEdit3(const char* label, bool initialize, glm::vec3* targetVal)
{
	if (initialize)
	{
		m_fColour[0] = targetVal->x;
		m_fColour[1] = targetVal->y;
		m_fColour[2] = targetVal->z;

		m_bEdited = false;
	}

	if (ImGui::ColorEdit3(label, m_fColour))
	{
		m_bEdited = true;
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(targetVal, glm::vec3(m_fColour[0], m_fColour[1], m_fColour[2]));
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

void NobleColourEdit::DoColourEdit4(const char* label, bool initialize, glm::vec4* targetVal)
{
	if (initialize)
	{
		m_fColour[0] = targetVal->x;
		m_fColour[1] = targetVal->y;
		m_fColour[2] = targetVal->z;
		m_fColour[3] = targetVal->w;

		m_bEdited = false;
	}

	if (ImGui::ColorEdit4(label, m_fColour))
	{
		m_bEdited = true;
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec4>* command = new ChangeValueCommand<glm::vec4>(targetVal, glm::vec4(m_fColour[0], m_fColour[1], m_fColour[2], m_fColour[3]));
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NobleDragFloat::DoDragFloat(const char* label, bool initialize, float* targetVal, float speed, float min, float max)
{
	if (initialize)
	{
		m_fValue[0] = *targetVal;

		m_bEdited = false;
	}

	if (ImGui::DragFloat(label, &m_fValue[0], speed, min, max))
	{
		m_bEdited = true;
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(targetVal, m_fValue[0]);
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

void NobleDragFloat::DoDragFloat2(const char* label, bool initialize, glm::vec2* targetVal, float speed)
{
	if (initialize)
	{
		m_fValue[0] = targetVal->x;
		m_fValue[1] = targetVal->y;

		m_bEdited = false;
	}

	if (ImGui::DragFloat2(label, &m_fValue[0], speed))
	{
		m_bEdited = true;
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec2>* command = new ChangeValueCommand<glm::vec2>(targetVal, glm::vec2(m_fValue[0], m_fValue[1]));
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

void NobleDragFloat::DoDragFloat3(const char* label, bool initialize, glm::vec3* targetVal, float speed)
{
	if (initialize)
	{
		m_fValue[0] = targetVal->x;
		m_fValue[1] = targetVal->y;
		m_fValue[2] = targetVal->z;

		m_bEdited = false;
	}

	if (ImGui::DragFloat3(label, &m_fValue[0], speed))
	{
		m_bEdited = true;
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(targetVal, glm::vec3(m_fValue[0], m_fValue[1], m_fValue[2]));
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

void NobleDragFloat::DoDragFloat4(const char* label, bool initialize, glm::vec4* targetVal, float speed)
{
	if (initialize)
	{
		m_fValue[0] = targetVal->x;
		m_fValue[1] = targetVal->y;
		m_fValue[2] = targetVal->z;
		m_fValue[3] = targetVal->w;

		m_bEdited = false;
	}

	if (ImGui::DragFloat4(label, &m_fValue[0], speed))
	{
		m_bEdited = true;
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec4>* command = new ChangeValueCommand<glm::vec4>(targetVal, glm::vec4(m_fValue[0], m_fValue[1], m_fValue[2], m_fValue[3]));
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NobleDragInt::DoDragInt(const char* label, bool initialize, int* targetVal, int speed, int min, int max)
{
	if (initialize)
	{
		m_iValue = *targetVal;

		m_bEdited = false;
	}

	if (ImGui::DragInt(label, &m_iValue, speed, min, max))
	{
		m_bEdited = true;
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<int>* command = new ChangeValueCommand<int>(targetVal, m_iValue);
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NobleTextInput::Initialize(std::string* targetVal)
{
	m_sValue = *targetVal;
	m_bEdited = false;
}

void NobleTextInput::DoTextInput(const char* label, bool initialize, std::string* targetVal)
{
	if (initialize)
	{
		Initialize(targetVal);
	}

	if(m_sID != "")
	{
		ImGui::PushID(m_sID.c_str());
	}

	if (ImGui::InputText(label, &m_sValue))
	{
		m_bEdited = true;
	}

	if(m_sID != "")
	{
		ImGui::PopID();
	}

	if (m_bEdited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<std::string>* command = new ChangeValueCommand<std::string>(targetVal, m_sValue);
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);

		m_bEdited = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void EntityDropdown::DoEntityDropdown(std::string ID, int index, std::string& selEntity, int layer)
{
	std::map<std::string, Entity>& entities = Application::GetApplication()->GetEntityList();
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	// Disable the default "open on single-click behavior" + set Selected flag according to our selection.
	// To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
	ImGuiTreeNodeFlags node_flags = base_flags;

	bool is_selected = false;
	Entity* pSelEntity = Application::GetApplication()->GetEntity(selEntity);
	if (pSelEntity != nullptr)
		is_selected = pSelEntity->m_sEntityID == entities.at(ID).m_sEntityID;
	if (is_selected)
		node_flags |= ImGuiTreeNodeFlags_Selected;

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)index, node_flags, entities.at(ID).m_sEntityName.c_str());
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		Application::GetApplication()->GetEditor()->SetSelectedEntity(entities.at(ID).m_sEntityID);
	}

	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::Button("Create Child Entity"))
		{
			AddEntityCommand* command = new AddEntityCommand("New Child Entity", entities.at(ID).m_sEntityID);
			Application::GetApplication()->PushCommand(command);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemTooltip("Create a new entity as a child of this entity.");
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			DeleteEntityCommand* command = new DeleteEntityCommand(entities.at(ID).m_sEntityID);
			Application::GetApplication()->PushCommand(command);

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemTooltip("Deletes the selected entity.");

		if (ImGui::BeginMenu("Add Component"))
		{
			std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();
			for (int o = 0; o < compRegistry->size(); o++)
			{
				if (ImGui::MenuItem(compRegistry->at(o).first.c_str()))
				{
					AddComponentCommand* command = new AddComponentCommand(entities.at(ID).m_sEntityID, o);
					Application::GetApplication()->PushCommand(command);
				}
			}

			ImGui::EndMenu();
		}

		if (ImGui::Button("Close"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	entities.at(ID).DoEntityInterface(index, node_open, selEntity, layer);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void ResourceSelectionWidget::DoResourceSelection(const char* label, bool initialize, std::shared_ptr<Resource> targetVal)
{
	ResourceManager* rManager = Application::GetApplication()->GetResourceManager();

	if (initialize)
	{
		Initialize(targetVal);
	}

	std::shared_ptr<Resource> newRes = rManager->DoResourceSelectInterface(label, m_pResource != nullptr ? m_pResource->m_sLocalPath : "none", m_sResourceType);

	if (newRes != nullptr && newRes != m_pResource)
	{
		ChangeResourceCommand* command = new ChangeResourceCommand(&targetVal, newRes);
		command->m_pComponent = m_pComponent;
		command->m_entity = m_pEntity;

		Application::GetApplication()->PushCommand(command);
	}
}