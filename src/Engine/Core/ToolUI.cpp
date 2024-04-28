#include "ToolUI.h"

#include "Application.h"
#include "ECS/Component.h"
#include "CommandTypes.h"

void NobleColourEdit::DoColourEdit3(const char* label, bool initialize, glm::vec3* targetVal, Component* comp)
{
	if (initialize)
	{
		m_fColour[0] = targetVal->x;
		m_fColour[1] = targetVal->y;
		m_fColour[2] = targetVal->z;

		edited = false;
	}

	if (ImGui::ColorEdit3(label, m_fColour))
	{
		edited = true;
	}

	if (edited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(targetVal, glm::vec3(m_fColour[0], m_fColour[1], m_fColour[2]), comp);
		Application::GetApplication()->PushCommand(command);

		edited = false;
	}
}

void NobleColourEdit::DoColourEdit4(const char* label, bool initialize, glm::vec4* targetVal, Component* comp)
{
	if (initialize)
	{
		m_fColour[0] = targetVal->x;
		m_fColour[1] = targetVal->y;
		m_fColour[2] = targetVal->z;
		m_fColour[3] = targetVal->w;

		edited = false;
	}

	if (ImGui::ColorEdit4(label, m_fColour))
	{
		edited = true;
	}

	if (edited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec4>* command = new ChangeValueCommand<glm::vec4>(targetVal, glm::vec4(m_fColour[0], m_fColour[1], m_fColour[2], m_fColour[3]), comp);
		Application::GetApplication()->PushCommand(command);

		edited = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NobleDragFloat::DoDragFloat(const char* label, bool initialize, float* targetVal, Component* comp, float speed, float min, float max)
{
	if (initialize)
	{
		m_fValue[0] = *targetVal;

		edited = false;
	}

	if (ImGui::DragFloat(label, &m_fValue[0], speed, min, max))
	{
		edited = true;
	}

	if (edited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<float>* command = new ChangeValueCommand<float>(targetVal, m_fValue[0], comp);
		Application::GetApplication()->PushCommand(command);

		edited = false;
	}
}

void NobleDragFloat::DoDragFloat2(const char* label, bool initialize, glm::vec2* targetVal, Component* comp, float speed)
{
	if (initialize)
	{
		m_fValue[0] = targetVal->x;
		m_fValue[1] = targetVal->y;

		edited = false;
	}

	if (ImGui::DragFloat2(label, &m_fValue[0], speed))
	{
		edited = true;
	}

	if (edited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec2>* command = new ChangeValueCommand<glm::vec2>(targetVal, glm::vec2(m_fValue[0], m_fValue[1]), comp);
		Application::GetApplication()->PushCommand(command);

		edited = false;
	}
}

void NobleDragFloat::DoDragFloat3(const char* label, bool initialize, glm::vec3* targetVal, Component* comp, float speed)
{
	if (initialize)
	{
		m_fValue[0] = targetVal->x;
		m_fValue[1] = targetVal->y;
		m_fValue[2] = targetVal->z;

		edited = false;
	}

	if (ImGui::DragFloat3(label, &m_fValue[0], speed))
	{
		edited = true;
	}

	if (edited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec3>* command = new ChangeValueCommand<glm::vec3>(targetVal, glm::vec3(m_fValue[0], m_fValue[1], m_fValue[2]), comp);
		Application::GetApplication()->PushCommand(command);

		edited = false;
	}
}

void NobleDragFloat::DoDragFloat4(const char* label, bool initialize, glm::vec4* targetVal, Component* comp, float speed)
{
	if (initialize)
	{
		m_fValue[0] = targetVal->x;
		m_fValue[1] = targetVal->y;
		m_fValue[2] = targetVal->z;
		m_fValue[3] = targetVal->w;

		edited = false;
	}

	if (ImGui::DragFloat4(label, &m_fValue[0], speed))
	{
		edited = true;
	}

	if (edited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<glm::vec4>* command = new ChangeValueCommand<glm::vec4>(targetVal, glm::vec4(m_fValue[0], m_fValue[1], m_fValue[2], m_fValue[3]), comp);
		Application::GetApplication()->PushCommand(command);

		edited = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void NobleDragInt::DoDragInt(const char* label, bool initialize, int* targetVal, Component* comp, int speed, int min, int max)
{
	if (initialize)
	{
		m_iValue = *targetVal;

		edited = false;
	}

	if (ImGui::DragInt(label, &m_iValue, speed, min, max))
	{
		edited = true;
	}

	if (edited && !ImGui::IsItemActive() && !ImGui::IsAnyItemActive())
	{
		ChangeValueCommand<int>* command = new ChangeValueCommand<int>(targetVal, m_iValue, comp);
		Application::GetApplication()->PushCommand(command);

		edited = false;
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------