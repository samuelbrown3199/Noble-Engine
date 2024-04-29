#pragma once

#include "../ECS/Component.h"
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

struct CameraBase
{
	ViewMode m_viewMode = projection;
	CameraState m_state = inactive;

	float m_fov = 90.0f;
	float m_scale = 50.0f;

	int m_iDrawMode = 1;

	virtual glm::vec3 GetPosition() = 0;
	virtual glm::vec3 GetRotation() = 0;

	virtual void SetPosition(glm::vec3 pos) {};
	virtual void SetRotation(glm::vec3 rot) {};
};

struct Camera : public Component, public CameraBase
{
	int m_camTransformIndex = -1;

	std::string GetComponentID() override
	{
		return "Camera";
	}

	void OnInitialize() override
	{
		m_fov = 90.0f;
		m_scale = 50.0f;
	}

	void OnRemove() override
	{
		m_camTransformIndex = -1;
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data = { {"CameraState", m_state}, {"ViewMode", m_viewMode}, {"FOV", m_fov}, {"Scale", m_scale}, {"DrawMode", m_iDrawMode} };
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
		if (j.find("Scale") != j.end())
			m_scale = j["Scale"];
		if(j.find("DrawMode") != j.end())
			m_iDrawMode = j["DrawMode"];
	}

	virtual void DoComponentInterface() override;
	virtual void PreUpdate() override;
	virtual void OnUpdate() override;

	virtual glm::vec3 GetPosition() override;
	virtual glm::vec3 GetRotation() override;

};