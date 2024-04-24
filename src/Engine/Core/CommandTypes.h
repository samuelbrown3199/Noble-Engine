#pragma once

#include <string>
#include <vector>

#include "ECS/Entity.h"

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