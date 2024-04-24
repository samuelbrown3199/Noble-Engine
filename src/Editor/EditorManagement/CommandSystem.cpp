#include "CommandSystem.h"


void CommandSystem::Undo()
{
	if(m_undoQueue.empty())
		return;

	Command* command = m_undoQueue.back();
	command->Undo();
	m_redoQueue.push_back(command);
	m_undoQueue.pop_back();
}

void CommandSystem::Redo()
{
	if(m_redoQueue.empty())
		return;

	Command* command = m_redoQueue.back();
	command->Redo();
	m_undoQueue.push_back(command);
	m_redoQueue.pop_back();
}

void CommandSystem::ProcessCommandQueue()
{
	while (!m_commandQueue.empty())
	{
		Command* command = m_commandQueue.front();
		command->Execute();
		m_undoQueue.push_back(command);
		m_commandQueue.pop();
	}
}

void CommandSystem::PushCommand(Command* command)
{ 
	m_commandQueue.push(command);

	// Clear the redo queue
	while (!m_redoQueue.empty())
	{
		delete m_redoQueue.back();
		m_redoQueue.pop_back();
	}
}