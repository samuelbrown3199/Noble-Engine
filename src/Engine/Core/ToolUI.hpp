#pragma once

#include "Logger.h"

#include "../Useful.h"
#include "../imgui/imgui.h"
#include "../imgui/implot.h"
#include "../imgui/implot_internal.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/ImGuiFileDialog.h"

class Editor;
class ToolUI;

class ToolModalWindow
{
	friend class ToolUI;
protected:

	std::string m_sID;

	ToolUI* m_pParentUI;
	bool m_bToggled = false;

	void CheckIfToggled()
	{
		if (m_bToggled)
			ImGui::OpenPopup(m_sID.c_str());

		m_bToggled = false;
	}

public:

	virtual void DoModal() = 0;
};

class ToolUI
{
protected:

	std::map<std::string, std::shared_ptr<ToolModalWindow>> m_mModalWindows;

	template<typename T>
	void AddModal(std::string ID)
	{
		std::shared_ptr<T> sys = std::make_shared<T>();
		sys->m_pParentUI = this;
		sys->m_sID = ID;

		m_mModalWindows[ID] = sys;
	}

public:

	bool m_uiOpen = false;
	ImGuiWindowFlags m_windowFlags = 0;
	ImGuiWindowFlags m_defaultFlags = 0;

	virtual void InitializeInterface(ImGuiWindowFlags defaultFlags) {}
	virtual void DoInterface() = 0;

	virtual void HandleShortcutInputs() {}

	ToolModalWindow* GetModal(std::string ID)
	{
		if (m_mModalWindows.count(ID) == 0)
			LogError(FormatString("Tried to get a modal window that doesnt exist, %s", ID.c_str()));

		return m_mModalWindows[ID].get();
	}

	void DoModal(std::string ID)
	{
		if (m_mModalWindows.count(ID) == 0)
			LogError(FormatString("Tried to open a modal window that doesnt exist, %s", ID.c_str()));

		m_mModalWindows[ID]->m_bToggled = true;
	}

	void DoModals()
	{
		std::map<std::string, std::shared_ptr<ToolModalWindow>>::iterator uiItr;
		for (uiItr = m_mModalWindows.begin(); uiItr != m_mModalWindows.end(); uiItr++)
		{
			uiItr->second->CheckIfToggled();
			uiItr->second->DoModal();
		}
	}

	virtual void UpdateWindowFlags() = 0;
};

class EditorToolUI : public ToolUI
{

protected:

	bool m_bUnsavedWork = false;
	bool m_bWindowFocused = false;
	bool m_bWindowHovered = false;

	ImVec2 m_windowPos;
	ImVec2 m_windowSize;
	ImVec2 m_relativeMousePos;

public:

	Editor* m_pEditor;

	virtual void InitializeInterface(ImGuiWindowFlags defaultFlags) { m_defaultFlags = defaultFlags; }
	virtual void DoInterface() = 0;

	virtual void HandleShortcutInputs() {}

	void SetUnsavedWork(bool b)
	{
		m_bUnsavedWork = b;
	}

	void UpdateWindowFlags()
	{
		m_windowFlags = m_defaultFlags;
		if(m_bUnsavedWork)
			m_windowFlags |= ImGuiWindowFlags_UnsavedDocument;
	}

	void UpdateWindowState()
	{
		m_bWindowFocused = ImGui::IsWindowFocused();
		m_bWindowHovered = ImGui::IsWindowHovered();

		m_windowPos = ImGui::GetWindowPos();
		m_windowSize = ImGui::GetWindowSize();
		m_relativeMousePos = ImVec2(ImGui::GetMousePos().x - m_windowPos.x, ImGui::GetMousePos().y - m_windowPos.y);

		if(m_bWindowFocused)
			HandleShortcutInputs();
	}

	bool IsWindowFocused() { return m_bWindowFocused; }
	bool IsWindowHovered() { return m_bWindowHovered; }
	ImVec2 GetWindowPos() { return m_windowPos; }
	ImVec2 GetWindowSize() { return m_windowSize; }
	ImVec2 GetRelativeMousePos() { return m_relativeMousePos; }
};