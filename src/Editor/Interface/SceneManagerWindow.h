#pragma

#include <Engine/Core/ToolUI.hpp>

class SceneManagerWindow : public EditorToolUI
{
private:

	std::vector<std::string> m_originalSceneOrder;

public:

	void UpdateOriginalSceneOrder();

	void InitializeInterface() override;
	void DoInterface() override;
};