#include "UILayout.h"

#include <fstream>

void UILayout::OnLoad()
{
	std::ifstream layoutFile;
	layoutFile.open(m_sResourcePath);
	std::string line;
	while (std::getline(layoutFile, line))
	{
		m_vLayoutLines.push_back(line);
	}
	layoutFile.close();
}