#pragma once

#include "../ResourceManager.h"
#include "../ECS/Component.h"
#include "Transform.h"

#include <glm/glm.hpp>

struct LightInfo
{
	glm::vec3 m_diffuse = glm::vec3(1, 1, 1);
	glm::vec3 m_specular = glm::vec3(1, 1, 1);

	virtual void DoLightInfoInterface()
	{
		float diffuse[3] = { m_diffuse.x, m_diffuse.y, m_diffuse.z };
		ImGui::ColorEdit3("Diffuse Colour", (float*)&diffuse);
		m_diffuse = glm::vec3(diffuse[0], diffuse[1], diffuse[2]);

		float specular[3] = { m_specular.x, m_specular.y, m_specular.z };
		ImGui::ColorEdit3("Specular Colour", (float*)&specular);
		m_specular = glm::vec3(specular[0], specular[1], specular[2]);
	};
	virtual nlohmann::json WriteInfoToJson() = 0;
	virtual void LoadInfoFromJson(nlohmann::json j) = 0;
};

struct DirectionalLight : public LightInfo
{
	glm::vec3 m_direction = glm::vec3(0.0f, 180.0f, 0.0f);
	float m_fIntensity = 1.0f;

	void DoLightInfoInterface()
	{
		ImGui::Text("Directional Light");
		ImGui::Dummy(ImVec2(0.0f, 3.0f));

		ImGui::BeginDisabled();
		float direction[3] = { m_direction.x, m_direction.y, m_direction.z };
		ImGui::DragFloat3("Direction", direction, 0.01f, -1, 1, "%.2f");
		ImGui::EndDisabled();

		ImGui::DragFloat("Intensity", &m_fIntensity, 0.01f, 0.0f, 1.0f, "%.2f");

		LightInfo::DoLightInfoInterface();
	}

	nlohmann::json WriteInfoToJson()
	{
		nlohmann::json data;

		data["Diffuse"] = { m_diffuse.x, m_diffuse.y, m_diffuse.z };
		data["Specular"] = { m_specular.x, m_specular.y, m_specular.z };

		return data;
	}

	void LoadInfoFromJson(nlohmann::json j)
	{
		m_diffuse = glm::vec3(j["Diffuse"][0], j["Diffuse"][1], j["Diffuse"][2]);
		m_specular = glm::vec3(j["Specular"][0], j["Specular"][1], j["Specular"][2]);
	}
};

struct PointLight : public LightInfo
{
	float m_constant = 1;
	float m_linear = 0.09f;
	float m_quadratic = 0.032f;

	virtual void DoLightInfoInterface()
	{
		ImGui::Text("Point Light");
		ImGui::Dummy(ImVec2(0.0f, 3.0f));

		ImGui::BeginDisabled();
		ImGui::DragFloat("Constant", &m_constant, 1.0f, 1.0f, 1.0f, "%.2f");
		ImGui::EndDisabled();

		ImGui::DragFloat("Linear", &m_linear, 0.01f, 0.0014f, 0.7f, "%.4f");
		ImGui::DragFloat("Quadratic", &m_quadratic, 0.01f, 0.000007f, 1.8f, "%.6f");

		LightInfo::DoLightInfoInterface();
	}

	virtual nlohmann::json WriteInfoToJson()
	{
		nlohmann::json data;

		data["Diffuse"] = { m_diffuse.x, m_diffuse.y, m_diffuse.z };
		data["Specular"] = { m_specular.x, m_specular.y, m_specular.z };

		data["constant"] = m_constant;
		data["linear"] = m_linear;
		data["quadratic"] = m_quadratic;

		return data;
	}

	virtual void LoadInfoFromJson(nlohmann::json j)
	{
		m_diffuse = glm::vec3(j["Diffuse"][0], j["Diffuse"][1], j["Diffuse"][2]);
		m_specular = glm::vec3(j["Specular"][0], j["Specular"][1], j["Specular"][2]);

		if (j.find("constant") != j.end())
			m_constant = j["constant"];
		if (j.find("linear") != j.end())
			m_linear = j["linear"];
		if (j.find("quadratic") != j.end())
			m_quadratic = j["quadratic"];
	}
};

struct SpotLight : public LightInfo
{
	float m_fCutOff = 12.5f;
	float m_fOuterCutOff = 13.5f;

	float m_constant = 1;
	float m_linear = 0.09f;
	float m_quadratic = 0.032f;

	virtual void DoLightInfoInterface()
	{
		ImGui::Text("Spot Light");
		ImGui::Dummy(ImVec2(0.0f, 3.0f));

		ImGui::BeginDisabled();
		ImGui::DragFloat("Constant", &m_constant, 1.0f, 1.0f, 1.0f, "%.2f");
		ImGui::EndDisabled();

		ImGui::DragFloat("Linear", &m_linear, 0.01f, 0.0014f, 0.7f, "%.4f");
		ImGui::DragFloat("Quadratic", &m_quadratic, 0.01f, 0.000007f, 1.8f, "%.6f");
		ImGui::DragFloat("Cutoff", &m_fCutOff);
		ImGui::DragFloat("Outer Cutoff", &m_fOuterCutOff);

		LightInfo::DoLightInfoInterface();
	}

	virtual nlohmann::json WriteInfoToJson()
	{
		nlohmann::json data;

		data["Diffuse"] = { m_diffuse.x, m_diffuse.y, m_diffuse.z };
		data["Specular"] = { m_specular.x, m_specular.y, m_specular.z };

		data["Cutoff"] = m_fCutOff;
		data["OuterCutoff"] = m_fOuterCutOff;
		data["constant"] = m_constant;
		data["linear"] = m_linear;
		data["quadratic"] = m_quadratic;

		return data;
	}

	virtual void LoadInfoFromJson(nlohmann::json j)
	{
		m_diffuse = glm::vec3(j["Diffuse"][0], j["Diffuse"][1], j["Diffuse"][2]);
		m_specular = glm::vec3(j["Specular"][0], j["Specular"][1], j["Specular"][2]);

		if (j.find("Cutoff") != j.end())
			m_fCutOff = j["Cutoff"];
		if (j.find("OuterCutoff") != j.end())
			m_fOuterCutOff = j["OuterCutoff"];

		if (j.find("constant") != j.end())
			m_constant = j["constant"];
		if (j.find("linear") != j.end())
			m_linear = j["linear"];
		if (j.find("quadratic") != j.end())
			m_quadratic = j["quadratic"];
	}
};





struct Light : public Component
{
	enum LightType
	{
		Point,
		Spot,
		Directional
	};
	LightType m_lightType;
	LightInfo* m_lightInfo;

	int m_transformIndex = -1;
	float m_fDistanceToCam;

	std::string GetComponentID() override
	{
		return "Light";
	}

	nlohmann::json WriteJson() override
	{
		nlohmann::json data;

		data["LightType"] = m_lightType;
		data["LightInfo"] = m_lightInfo->WriteInfoToJson();

		return data;
	}

	void FromJson(const nlohmann::json& j) override
	{
		ChangeLightType((LightType)j["LightType"]);

		CreateLightInfo();
		m_lightInfo->LoadInfoFromJson(j["LightInfo"]);
	}

	void OnInitialize()
	{
		m_lightType = Point;
		m_lightInfo = new PointLight();
	}

	void OnRemove() override
	{
		m_transformIndex = -1;
	}

	void ChangeLightType(LightType newType)
	{
		if (newType == m_lightType)
			return;

		m_lightType = newType;
		delete m_lightInfo;
		CreateLightInfo();
	}

	void CreateLightInfo()
	{
		switch (m_lightType)
		{
		case Directional:
			m_lightInfo = new DirectionalLight();
			break;
		case Point:
			m_lightInfo = new PointLight();
			break;
		case Spot:
			m_lightInfo = new SpotLight();
			break;
		default:
			LogError("Trying to switch light to unsupported type.");
			break;
		}
	}

	virtual void DoComponentInterface() override
	{
		if (m_transformIndex == -1)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("No transform attached. Light won't render.");
			ImGui::PopStyleColor();

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		}

		const char* lightTypes[] = { "Point", "Spot NYI", "Directional"};
		int selLightType = m_lightType;
		ImGui::Combo("Light Type", &selLightType, lightTypes, IM_ARRAYSIZE(lightTypes));
		ChangeLightType((LightType)selLightType);
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		ImGui::Text("Light Information");
		if (m_lightInfo)
			m_lightInfo->DoLightInfoInterface();
	}

	virtual void PreUpdate() override;
	virtual void OnUpdate() override;
};