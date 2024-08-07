#include "CommandTypes.h"
#include "Registry.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AddEntityCommand::Execute()
{
	m_entities.push_back(*Application::GetApplication()->CreateEntity("", m_sEntityName, m_sParentID));

	if(!m_sParentID.empty())
		Application::GetApplication()->LinkChildEntities();
}

void AddEntityCommand::Undo()
{
	for (int i = 0; i < m_entities.size(); i++)
		Application::GetApplication()->DeleteEntity(m_entities.at(i).m_sEntityID);
}

void AddEntityCommand::Redo()
{
	for (int i = 0; i < m_entities.size(); i++)
		Application::GetApplication()->CreateEntity(m_entities.at(i).m_sEntityID, m_entities.at(i).m_sEntityName, m_entities.at(i).m_sEntityParentID);

	if (!m_sParentID.empty())
		Application::GetApplication()->LinkChildEntities();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DeleteEntityCommand::DeleteEntityCommand(std::string sEntityID)
{
	m_entities.push_back(*Application::GetApplication()->GetEntity(sEntityID));

	//First pass to get all child objects.
	for (int i = 0; i < m_entities.size(); i++)
	{
		for(int j = 0; j < m_entities.at(i).m_vChildEntityIDs.size(); j++)
			m_entities.push_back(*Application::GetApplication()->GetEntity(m_entities.at(i).m_vChildEntityIDs.at(j)));
	}

	//Second pass to get all components and behaviours.
	for (int i = 0; i < m_entities.size(); i++)
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();

		std::vector<Component*> components;

		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();
		for (int j = 0; j < compRegistry->size(); j++)
		{
			int compIndex = compRegistry->at(j).second.m_componentDatalist->GetComponentIndex(m_entities.at(i).m_sEntityID);
			if(compIndex != -1)
				components.push_back(compRegistry->at(j).second.m_componentDatalist->GetComponent(compIndex));
		}

		m_entityComponents.push_back(components);
	}
}

void DeleteEntityCommand::Execute()
{
	for (int i = 0; i < m_entities.size(); i++)
	{
		Application::GetApplication()->DeleteEntity(m_entities.at(i).m_sEntityID);
	}
}

void DeleteEntityCommand::Undo()
{
	for (int i = 0; i < m_entities.size(); i++)
	{
		Entity* newEnt = Application::GetApplication()->CreateEntity(m_entities.at(i).m_sEntityID, m_entities.at(i).m_sEntityName, m_entities.at(i).m_sEntityParentID);

		for (int j = 0; j < m_entityComponents.at(i).size(); j++)
		{
			m_entityComponents.at(i).at(j)->m_sEntityID = newEnt->m_sEntityID;
			m_entityComponents.at(i).at(j)->m_bAvailableForReuse = false;
			m_entityComponents.at(i).at(j)->AddComponent();
		}

		newEnt->GetAllComponents();
	}

	Application::GetApplication()->LinkChildEntities(); //kinda slow going through all entities, but it's the only way to ensure that all entities are linked correctly currently.
}

void DeleteEntityCommand::Redo()
{
	Execute();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AddComponentCommand::Execute()
{
	std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();
	compRegistry->at(m_registryIndex).second.m_comp->AddComponentToEntity(m_sEntityID);

	m_component = compRegistry->at(m_registryIndex).second.m_componentDatalist->GetComponent(compRegistry->at(m_registryIndex).second.m_componentDatalist->GetComponentIndex(m_sEntityID));
	m_component->m_bInitializeInterface = true;
}

void AddComponentCommand::Undo()
{
	m_component->RemoveComponent(m_sEntityID);
}

void AddComponentCommand::Redo()
{
	m_component->m_sEntityID = m_sEntityID;
	m_component->m_bAvailableForReuse = false;
	m_component->AddComponent();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void RemoveComponentCommand::Execute()
{
	std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();
	m_component = compRegistry->at(m_registryIndex).second.m_componentDatalist->GetComponent(compRegistry->at(m_registryIndex).second.m_componentDatalist->GetComponentIndex(m_sEntityID));

	m_component->RemoveComponent(m_sEntityID);
}

void RemoveComponentCommand::Undo()
{
	m_component->m_sEntityID = m_sEntityID;
	m_component->m_bAvailableForReuse = false;
	m_component->AddComponent();
}

void RemoveComponentCommand::Redo()
{
	m_component->RemoveComponent(m_sEntityID);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Entity* EntityCopy::ProcessEntityCopy()
{
	Entity* copiedEntity = Application::GetApplication()->CreateEntity("", m_sName + "_Copy", m_sParentID);

	std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = Application::GetApplication()->GetRegistry()->GetComponentRegistry();
	std::map<std::string, int>::iterator it;
	for (it = m_vComponents.begin(); it != m_vComponents.end(); it++)
	{
		for (int i = 0; i < compRegistry->size(); i++)
		{
			if (compRegistry->at(i).first == it->first)
			{
				Component* compToCopy = compRegistry->at(i).second.m_componentDatalist->GetComponent(it->second);
				Component* pastedComp = compRegistry->at(i).second.m_comp->CopyComponent(compToCopy, copiedEntity->m_sEntityID);

				break;
			}
		}
	}

	copiedEntity->GetAllComponents();

	//TODO: rework when we have multiple selection.
	if (Application::GetApplication()->GetEditor() != nullptr && copiedEntity->m_sEntityParentID == "")
		Application::GetApplication()->GetEditor()->SetSelectedEntity(copiedEntity->m_sEntityID);

	for(int i = 0; i < m_vChildrenCopy.size(); i++)
	{
		m_vChildrenCopy.at(i).m_sParentID = copiedEntity->m_sEntityID;
		Entity* childCopy = m_vChildrenCopy.at(i).ProcessEntityCopy();
	}

	return copiedEntity;
}

void CopyEntityCommand::Execute()
{
	for (int i = 0; i < m_vEntityCopies.size(); i++)
	{
		m_vCopiedEntities.push_back(m_vEntityCopies.at(i)->ProcessEntityCopy());
	}

	Application::GetApplication()->LinkChildEntities();
}

void CopyEntityCommand::Undo()
{
	for(int i = 0; i < m_vCopiedEntities.size(); i++)
		Application::GetApplication()->DeleteEntity(m_vCopiedEntities.at(i)->m_sEntityID);
}

void CopyEntityCommand::Redo()
{
	Execute();
}