#pragma once

#include <string>
#include <vector>

#include "ECS/Entity.h"
#include "ECS/Component.h"

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

	EntityCommand() {};

	virtual void Execute() = 0;
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};

struct AddEntityCommand : public EntityCommand
{
	std::string m_sEntityName;
	std::string m_sParentID;

	AddEntityCommand(std::string name, std::string parentID) : EntityCommand() 
	{
		m_sEntityName = name;
		m_sParentID = parentID;
	}

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

struct AddComponentCommand : public Command
{
	std::string m_sEntityID;
	int m_registryIndex;

	Component* m_component;

	AddComponentCommand(std::string sEntityID, int registryIndex)
	{
		m_sEntityID = sEntityID;
		m_registryIndex = registryIndex;

		m_component = nullptr;
	}

	void Execute() override;
	void Undo() override;
	void Redo() override;
};

struct RemoveComponentCommand : public Command
{
	std::string m_sEntityID;
	int m_registryIndex;

	Component* m_component;

	RemoveComponentCommand(std::string sEntityID, int registryIndex)
	{
		m_sEntityID = sEntityID;
		m_registryIndex = registryIndex;

		m_component = nullptr;
	}

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

	Component* m_pComponent;

	ChangeValueCommand(T* target, T newValue, Component* comp = nullptr)
	{
		m_pTargetValue = target;
		m_newValue = newValue;
		m_oldValue = *target;
		m_pComponent = comp;
	}

	void Execute() override
	{
		*m_pTargetValue = m_newValue;

		if(m_pComponent != nullptr)
			m_pComponent->m_bInitializeInterface = true;
	}

	void Undo()
	{
		*m_pTargetValue = m_oldValue;

		if (m_pComponent != nullptr)
			m_pComponent->m_bInitializeInterface = true;
	}

	void Redo() override
	{
		Execute();
	}
};