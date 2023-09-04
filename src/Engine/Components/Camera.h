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

	float m_fov;

	static ComponentDatalist<Camera> m_componentList;

	void OnInitialize() override
	{
		m_fov = 90.0f;
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"CameraState", m_state}, {"ViewMode", m_viewMode}, {"FOV", m_fov} };
		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		if (j.find("CameraState") != j.end())
			m_state = j["CameraState"];
		if(j.find("ViewMode") != j.end())
			m_viewMode = j["ViewMode"];
		if (j.find("FOV") != j.end())
			m_fov = j["FOV"];
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

		ImGui::DragFloat("FoV", &m_fov, 0.5f, 20.0f, 150.0f, "%.2f");
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