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
		Application::GetApplication()->DeleteEntity(Application::GetApplication()->GetEntityIndex(m_entities.at(i).m_sEntityID));
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
	m_entities.push_back(*Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(sEntityID)));

	//First pass to get all child objects.
	for (int i = 0; i < m_entities.size(); i++)
	{
		for(int j = 0; j < m_entities.at(i).m_vChildEntityIDs.size(); j++)
			m_entities.push_back(*Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(m_entities.at(i).m_vChildEntityIDs.at(j))));
	}

	//Second pass to get all components and behaviours.
	for (int i = 0; i < m_entities.size(); i++)
	{
		NobleRegistry* registry = Application::GetApplication()->GetRegistry();

		std::vector<Component*> components;
		std::vector<Behaviour*> behaviours;

		std::vector<std::pair<std::string, ComponentRegistry>>* compRegistry = registry->GetComponentRegistry();
		for (int j = 0; j < compRegistry->size(); j++)
		{
			int compIndex = compRegistry->at(j).second.m_componentDatalist->GetComponentIndex(m_entities.at(i).m_sEntityID);
			if(compIndex != -1)
				components.push_back(compRegistry->at(j).second.m_componentDatalist->GetComponent(compIndex));
		}

		std::vector<Behaviour*> entityBehaviours = m_entities.at(i).GetBehaviours();
		for (int j = 0; j < entityBehaviours.size(); j++)
			behaviours.push_back(entityBehaviours.at(j));

		m_entityComponents.push_back(components);
		m_entityBehaviours.push_back(behaviours);
	}
}

void DeleteEntityCommand::Execute()
{
	for (int i = 0; i < m_entities.size(); i++)
	{
		Application::GetApplication()->DeleteEntity(Application::GetApplication()->GetEntityIndex(m_entities.at(i).m_sEntityID));
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

		for (int j = 0; j < m_entityBehaviours.at(i).size(); j++)
			newEnt->AddBehaviour(m_entityBehaviours.at(i).at(j));

		newEnt->GetAllComponents();
	}

	Application::GetApplication()->LinkChildEntities(); //kinda slow going through all entities, but it's the only way to ensure that all entities are linked correctly currently.
}

void DeleteEntityCommand::Redo()
{
	Execute();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------