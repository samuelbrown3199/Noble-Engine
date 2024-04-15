#include "NobleDLL.h"

#include "Application.h"
#include "Logger.h"

typedef int(__stdcall* DLLFunc)(std::shared_ptr<Application>);

NobleDLL::NobleDLL(std::string path)
{
	m_sDLLPath = path;
	m_hDLLInstance = NULL;
}

void NobleDLL::LoadDLL(std::shared_ptr<Application> application)
{
	// Load the DLL
	m_hDLLInstance = LoadLibrary(m_sDLLPath.c_str());

	// Check if the DLL was loaded
	if (m_hDLLInstance == NULL)
	{
		Logger::LogError("Failed to load DLL: " + m_sDLLPath, 1);
	}
	else
	{
		Logger::LogInformation("DLL loaded: " + m_sDLLPath);

		DLLFunc func = (DLLFunc)GetProcAddress(m_hDLLInstance, "dllMain");
		if (!func)
		{
			Logger::LogError("Could not find dllMain function in game dll" , 2);
		}
		else
		{
			func(application);
		}
	}
}