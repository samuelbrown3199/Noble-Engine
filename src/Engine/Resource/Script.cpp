#include "Script.h"

#include <fstream>
#include <sstream>

Script::Script()
{
	m_resourceType = "Script";
}

void Script::OnLoad()
{
	std::ifstream scriptFile(m_sResourcePath);
	if (scriptFile.is_open())
	{
		std::stringstream buffer;
		buffer << scriptFile.rdbuf();

		m_sScriptString = buffer.str();
	}
}

void Script::OnUnload()
{
	m_sScriptString.clear();
}