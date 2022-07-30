#include <Engine/Core/UI.h>

struct MainMenu : public UISystem
{
private:

	std::shared_ptr<UIButton> m_openServerButton, m_joinServerButton;

public:

	MainMenu(const std::string _UILayout) : UISystem(_UILayout)
	{

	}

	void InitializeSpecificElements();
	void HandleEvents();
};