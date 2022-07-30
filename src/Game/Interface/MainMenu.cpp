#include <Engine/Core/Application.h>

#include "MainMenu.h"

void MainMenu::InitializeSpecificElements()
{
	m_openServerButton = GetElementByID<UIButton>("StartServerButton");
}

void MainMenu::HandleEvents()
{
	if (m_openServerButton->ClickedOn())
	{
		//Application::CreateNetworkManager(0);
	}
	/*if (m_joinServerButton->ClickedOn())
	{
		Application::CreateNetworkManager(1);
	}*/
}