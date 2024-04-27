#pragma

#include <Engine/Core/ToolUI.h>

class DeleteSceneModal : public ToolModalWindow
{
public:

	int m_selectedSceneIndex = -1;

	void DoModal() override;
};

class RenameSceneModal : public ToolModalWindow
{
public:
	int m_selectedSceneIndex = -1;

	void DoModal() override;

};

class SceneManagerWindow : public EditorToolUI
{
private:

	std::vector<std::string> m_originalSceneOrder;
	int m_selectedSceneIndex = -1;

public:

	void UpdateOriginalSceneOrder();

	void InitializeInterface(ImGuiWindowFlags defaultFlags) override;
	void DoInterface() override;
};