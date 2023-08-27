#pragma once

#include "../Core/ResourceManager.h"

#include "../ECS/ComponentList.hpp"
#include "../ECS/Component.hpp"

#include "Transform.h"

#include <glm/glm.hpp>

struct LightInfo
{
	glm::vec3 m_diffuse = glm::vec3(1, 1, 1);
	glm::vec3 m_specular = glm::vec3(1, 1, 1);

	virtual void BindInfoToShaders(int curLight, Transform* tr) = 0;
	virtual void DoLightInfoInterface()
	{
		ImVec4 diffuse = ImVec4(m_diffuse.x, m_diffuse.y, m_diffuse.z, 1.0f);
		ImGui::ColorEdit4("Diffuse Colour", (float*)&diffuse);
		m_diffuse = glm::vec4(diffuse.x, diffuse.y, diffuse.z, diffuse.w);

		ImVec4 specular = ImVec4(m_specular.x, m_specular.y, m_specular.z, 1.0f);
		ImGui::ColorEdit4("Specular Colour", (float*)&specular);
		m_specular = glm::vec4(specular.x, specular.y, specular.z, specular.w);
	};
	virtual nlohmann::json WriteInfoToJson() = 0;
	virtual void LoadInfoFromJson(nlohmann::json j) = 0;
};

struct DirectionalLight : public LightInfo
{
	glm::vec3 m_direction;

	void BindInfoToShaders(int curLight, Transform* tr)
	{
		Logger::LogInformation("Using directional light, which is not implemented in this version of the engine.");
	}

	void DoLightInfoInterface()
	{
		ImGui::Text("Directional Light, NYI");
		ImGui::Dummy(ImVec2(0.0f, 3.0f));

		ImGui::BeginDisabled();
		float direction[3] = { m_direction.x, m_direction.y, m_direction.z };
		ImGui::DragFloat3("Direction", direction, 0.01f, -1, 1, "%.2f");
		ImGui::EndDisabled();

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

	virtual void BindInfoToShaders(int curLight, Transform* tr)
	{
		std::vector<std::shared_ptr<ShaderProgram>>* shaderPrograms = ResourceManager::GetShaderPrograms();
		for (int i = 0; i < shaderPrograms->size(); i++)
		{
			shaderPrograms->at(i)->UseProgram();

			std::string location = FormatString("lights[%d]", curLight);

			shaderPrograms->at(i)->BindVector3(location + ".direction", tr->m_rotation);
			shaderPrograms->at(i)->BindVector3(location + ".position", tr->m_position);

			shaderPrograms->at(i)->BindVector3(location + ".diffuseLight", m_diffuse);
			shaderPrograms->at(i)->BindVector3(location + ".specularLight", m_specular);
			shaderPrograms->at(i)->BindFloat(location + ".constant", m_constant);
			shaderPrograms->at(i)->BindFloat(location + ".linear", m_linear);
			shaderPrograms->at(i)->BindFloat(location + ".quadratic", m_quadratic);
			//Application::standardShader->BindFloat(location + "].intensity", closestLights.at(i)->intensity);
		}
	}

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

		if(j.find("constant") != j.end())
			m_constant = j["constant"];
		if (j.find("linear") != j.end())
			m_linear = j["linear"];
		if (j.find("quadratic") != j.end())
			m_quadratic = j["quadratic"];
	}
};

struct Light : public Component
{
	static ComponentDatalist<Light> m_componentList;
	static int m_iCurrentLight;
	static std::vector<Light*> m_closestLights;

	Transform* m_transform;

	enum LightType
	{
		Directional,
		Point
	};
	LightType m_lightType;
	LightInfo* m_lightInfo = nullptr;

	float m_fDistanceToCam;

	Component* GetAsComponent(std::string entityID) override
	{
		return GetComponent(entityID);
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
		default:
			Logger::LogError("Trying to switch light to unsupported type.", 2);
			break;
		}
	}

	virtual void DoComponentInterface() override
	{
		if (m_transform == nullptr)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
			ImGui::Text("No transform attached. Light won't render.");
			ImGui::PopStyleColor();

			ImGui::Dummy(ImVec2(0.0f, 5.0f));
		}

		const char* lightTypes[] = { "Directional", "Point"};
		int selLightType = m_lightType;
		ImGui::Combo("Light Type", &selLightType, lightTypes, IM_ARRAYSIZE(lightTypes));
		ChangeLightType((LightType)selLightType);
		ImGui::Dummy(ImVec2(0.0f, 5.0f));

		ImGui::Text("Light Information");
		if (m_lightInfo)
			m_lightInfo->DoLightInfoInterface();
	}

	virtual void AddComponent() override;
	virtual void AddComponentToEntity(std::string entityID) override;
	virtual void RemoveComponent(std::string entityID) override;
	virtual void RemoveAllComponents() override;

	Light* GetComponent(std::string entityID);

	virtual void PreUpdate();
	virtual void Update(bool useThreads, int maxComponentsPerThread) override;
	virtual void OnUpdate() override;

	virtual void Render(bool useThreads, int maxComponentsPerThread) override;

	virtual void LoadComponentDataFromJson(nlohmann::json& j) override;
	virtual nlohmann::json WriteComponentDataToJson() override;
};