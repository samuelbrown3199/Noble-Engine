#pragma

#include <Engine/Core/ToolUI.hpp>

class SceneManagerWindow : public EditorToolUI
{
private:

	std::vector<std::string> m_originalSceneOrder;
	int m_selectedSceneIndex = -1;

public:

	void UpdateOriginalSceneOrder();

	void InitializeInterface() override;
	void DoInterface() override;
};