#pragma once

#include <queue>

#include <Engine/Core/Application.h>
#include <Engine/Core/CommandTypes.h>

class CommandSystem
{
private:

	std::deque<Command*> m_undoQueue;
	std::deque<Command*> m_redoQueue;

	std::queue<Command*> m_commandQueue;

public:

	void Undo();
	void Redo();

	void ProcessCommandQueue();
	void PushCommand(Command* command);
};