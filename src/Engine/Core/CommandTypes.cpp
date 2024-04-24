#include "CommandTypes.h"

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AddEntityCommand::Execute()
{
	m_entities.push_back(*Application::GetApplication()->CreateEntity());
}

void AddEntityCommand::Undo()
{
	for (int i = 0; i < m_entities.size(); i++)
		Application::GetApplication()->DeleteEntity(Application::GetApplication()->GetEntityIndex(m_entities.at(i).m_sEntityID));
}

void AddEntityCommand::Redo()
{
	for(int i = 0; i < m_entities.size(); i++)
		Application::GetApplication()->CreateEntity(m_entities.at(i).m_sEntityID, m_entities.at(i).m_sEntityName, m_entities.at(i).m_sEntityParentID);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DeleteEntityCommand::DeleteEntityCommand(std::string sEntityID)
{
	m_entities.push_back(*Application::GetApplication()->GetEntity(Application::GetApplication()->GetEntityIndex(sEntityID)));
}

void DeleteEntityCommand::Execute()
{
	for (int i = 0; i < m_entities.size(); i++)
		Application::GetApplication()->DeleteEntity(Application::GetApplication()->GetEntityIndex(m_entities.at(i).m_sEntityID));
}

void DeleteEntityCommand::Undo()
{
	for (int i = 0; i < m_entities.size(); i++)
		Application::GetApplication()->CreateEntity(m_entities.at(i).m_sEntityID, m_entities.at(i).m_sEntityName, m_entities.at(i).m_sEntityParentID);
}

void DeleteEntityCommand::Redo()
{
	for (int i = 0; i < m_entities.size(); i++)
		Application::GetApplication()->DeleteEntity(Application::GetApplication()->GetEntityIndex(m_entities.at(i).m_sEntityID));
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------