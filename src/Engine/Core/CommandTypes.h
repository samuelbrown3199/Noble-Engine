#pragma once

#include <string>
#include <vector>

#include "ECS/Entity.h"
#include "ECS/Component.h"
#include "ECS/Behaviour.hpp"

struct Command
{
	Command() {}

	virtual void Execute() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};


struct EntityCommand : public Command
{
	std::vector<Entity> m_entities;
	std::vector<std::vector<Component*>> m_entityComponents;
	std::vector<std::vector<Behaviour*>> m_entityBehaviours;

	EntityCommand() {};

	virtual void Execute() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};

struct AddEntityCommand : public EntityCommand
{
	AddEntityCommand() : EntityCommand() { }

	void Execute() override;
	void Undo() override;
	void Redo() override;
};

struct DeleteEntityCommand : public EntityCommand
{
	DeleteEntityCommand(std::string sEntityID);

	void Execute() override;
	void Undo() override;
	void Redo() override;
};

template<typename T>
struct ChangeValueCommand : public Command
{
	T* m_pTargetValue;
	T m_newValue;
	T m_oldValue;

	ChangeValueCommand(T* target, T newValue)
	{
		m_pTargetValue = target;
		m_newValue = newValue;
		m_oldValue = *target;
	}

	void Execute() override
	{
		*m_pTargetValue = m_newValue;
	}

	void Undo()
	{
		*m_pTargetValue = m_oldValue;
	}

	void Redo() override
	{
		Execute();
	}
};