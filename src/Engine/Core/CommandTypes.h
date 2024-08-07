#pragma once

#include <string>
#include <vector>

#include "ResourceManager.h"
#include "EngineResources/Resource.h"
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

	Component* m_pComponent = nullptr;
	Entity* m_entity = nullptr;
	Resource* m_resource = nullptr;

	ChangeValueCommand(T* target, T newValue)
	{
		m_pTargetValue = target;
		m_newValue = newValue;
		m_oldValue = *target;
	}

	void Execute() override
	{
		*m_pTargetValue = m_newValue;

		if(m_pComponent != nullptr)
			m_pComponent->m_bInitializeInterface = true;

		if (m_entity != nullptr)
			m_entity->m_bInitializeInterface = true;

		if (m_resource != nullptr)
			m_resource->m_bInitializeInterface = true;
	}

	void Undo()
	{
		*m_pTargetValue = m_oldValue;

		if (m_pComponent != nullptr)
			m_pComponent->m_bInitializeInterface = true;

		if (m_entity != nullptr)
			m_entity->m_bInitializeInterface = true;

		if(m_resource != nullptr)
			m_resource->m_bInitializeInterface = true;
	}

	void Redo() override
	{
		Execute();
	}
};

struct ChangeResourceCommand : public Command
{
	std::shared_ptr<Resource>* m_pTargetValue;
	std::string m_newValue;
	std::string m_oldValue;

	Component* m_pComponent = nullptr;
	Entity* m_entity = nullptr;
	Resource* m_resource = nullptr;
	
	ChangeResourceCommand(std::shared_ptr<Resource>* target, std::string newValue)
	{
		m_pTargetValue = target;
		m_newValue = newValue;
		m_oldValue = m_pTargetValue->get()->m_sLocalPath;
	}

	void Execute() override
	{
		ResourceManager* rManager = Application::GetApplication()->GetResourceManager();
		*m_pTargetValue = rManager->LoadResource<Resource>(m_newValue);

		if (m_pComponent != nullptr)
			m_pComponent->m_bInitializeInterface = true;

		if (m_entity != nullptr)
			m_entity->m_bInitializeInterface = true;

		if (m_resource != nullptr)
			m_resource->m_bInitializeInterface = true;
	}

	void Undo()
	{
		ResourceManager* rManager = Application::GetApplication()->GetResourceManager();
		*m_pTargetValue = rManager->LoadResource<Resource>(m_oldValue);

		if (m_pComponent != nullptr)
			m_pComponent->m_bInitializeInterface = true;

		if (m_entity != nullptr)
			m_entity->m_bInitializeInterface = true;

		if (m_resource != nullptr)
			m_resource->m_bInitializeInterface = true;
	}

	void Redo() override
	{
		Execute();
	}
};

struct EntityCopy
{
	std::string m_sName = "";
	std::string m_sParentID = "";
	std::map<std::string, int> m_vComponents;
	std::vector<EntityCopy> m_vChildrenCopy;

	EntityCopy(Entity* entity)
	{
		m_sName = entity->m_sEntityName;
		m_sParentID = entity->m_sEntityParentID;
		m_vComponents = entity->m_vComponents;

		for (std::string child : entity->m_vChildEntityIDs)
		{
			EntityCopy childCopy = EntityCopy(Application::GetApplication()->GetEntity(child));
			m_vChildrenCopy.push_back(childCopy);
		}
	}

	Entity* ProcessEntityCopy();
};

struct CopyEntityCommand : public Command
{
	std::vector<EntityCopy*> m_vEntityCopies;
	std::vector<Entity*> m_vCopiedEntities;

	CopyEntityCommand(std::vector<EntityCopy*> entities)
	{
		m_vEntityCopies = entities;
	}

	void Execute() override;
	void Undo() override;
	void Redo() override;
};