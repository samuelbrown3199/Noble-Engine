#pragma once

#include "../ECS/ComponentList.hpp"
#include "../ECS/Component.hpp"

#include <glm/glm.hpp>

struct Transform : Component
{
	glm::vec3 m_prevPosition;
	glm::vec3 m_prevRotation;
	glm::vec3 m_prevScale;

	glm::vec3 m_position;
	glm::vec3 m_rotation;
	glm::vec3 m_scale;

	glm::mat4 m_transformMat;

	static ComponentDatalist<Transform> m_componentList;

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"position",{m_position.x, m_position.y, m_position.z}}, {"rotation",{m_rotation.x, m_rotation.y, m_rotation.z}}, {"scale",{m_scale.x, m_scale.y, m_scale.z}} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_position = glm::vec3(j.at("position")[0], j.at("position")[1], j.at("position")[2]);
		m_rotation = glm::vec3(j.at("rotation")[0], j.at("rotation")[1], j.at("rotation")[2]);
		m_scale= glm::vec3(j.at("scale")[0], j.at("scale")[1], j.at("scale")[2]);
	}

	void OnInitialize()
	{
		m_position = glm::vec3(0, 0, 0);
		m_rotation = glm::vec3(0, 0, 0);
		m_scale = glm::vec3(1, 1, 1);
	}

	void OnInitialize(const glm::vec3& _pos)
	{
		m_position = _pos;
		m_rotation = glm::vec3(0, 0, 0);
		m_scale = glm::vec3(1, 1, 1);
	}

	void OnInitialize(const glm::vec3& _pos, const glm::vec3& _rot) 
	{
		m_position = _pos;
		m_rotation = _rot;
		m_scale = glm::vec3(1, 1, 1);
	}

	void OnInitialize(const glm::vec3& _pos, const glm::vec3& _rot, const glm::vec3& _sca)
	{
		m_position = _pos;
		m_rotation = _rot;
		m_scale = _sca;
	}

	virtual void DoComponentInterface() override
	{
		float position[3] = { m_position.x, m_position.y, m_position.z};
		ImGui::DragFloat3("Position", position, 0.01f);
		m_position = glm::vec3(position[0], position[1], position[2]);

		float rotation[3] = { m_rotation.x, m_rotation.y, m_rotation.z };
		ImGui::DragFloat3("Rotation", rotation, 0.01f);
		m_rotation = glm::vec3(rotation[0], rotation[1], rotation[2]);

		float scale[3] = { m_scale.x, m_scale.y, m_scale.z };
		ImGui::DragFloat3("Scale", scale, 0.01f);
		m_scale = glm::vec3(scale[0], scale[1], scale[2]);
	}

	virtual void AddComponent() override;
	virtual void AddComponentToEntity(std::string entityID) override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	Transform* GetComponent(std::string entityID);

	virtual void Update(bool useThreads, int maxComponentsPerThread) override;
	virtual void OnUpdate() override;

	virtual void Render(bool useThreads, int maxComponentsPerThread) override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};