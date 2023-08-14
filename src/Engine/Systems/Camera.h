#pragma once

#include "../ECS/ComponentList.hpp"
#include "../ECS/Component.hpp"
#include "Transform.h"

enum CameraState
{
	inactive,
	mainCam,
	editorCam
};

enum ViewMode
{
	projection,
	orthographic
};

struct Camera : Component
{
	Transform* m_camTransform = nullptr;
	CameraState m_state = inactive;
	ViewMode m_viewMode = projection;

	static ComponentDatalist<Camera> m_componentList;

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"CameraState", m_state}, {"ViewMode", m_viewMode} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		m_state = j["CameraState"];
		m_viewMode = j["ViewMode"];
	}

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
	}

	virtual void DoComponentInterface() override
	{
		const char* states[] = { "Inactive", "Main Cam", "Editor Cam" };
		int currentState = m_state;
		ImGui::Combo("State", &currentState, states, IM_ARRAYSIZE(states));
		m_state = (CameraState)currentState;

		const char* viewmodes[] = { "Projection", "Orthographic" };
		int currentViewMode = m_viewMode;
		ImGui::Combo("View Mode", &currentViewMode, viewmodes, IM_ARRAYSIZE(viewmodes));
		m_viewMode = (ViewMode)currentViewMode;
	}

	virtual void AddComponent() override;
	virtual void AddComponentToEntity(std::string entityID) override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	Camera* GetComponent(std::string entityID);

	virtual void PreUpdate() override;
	virtual void Update(bool useThreads, int maxComponentsPerThread) override;
	virtual void OnUpdate() override;

	virtual void Render(bool useThreads, int maxComponentsPerThread) override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};