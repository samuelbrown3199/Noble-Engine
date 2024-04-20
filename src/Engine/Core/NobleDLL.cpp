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
		LogError("Failed to load DLL: " + m_sDLLPath);
	}
	else
	{
		LogInfo("DLL loaded: " + m_sDLLPath);

		DLLFunc func = (DLLFunc)GetProcAddress(m_hDLLInstance, "dllMain");
		if (!func)
		{
			LogFatalError("Could not find dllMain function in game dll");
		}
		else
		{
			func(application);
		}
	}
}

void NobleDLL::UnloadDLL()
{
	if (m_hDLLInstance != NULL)
	{
		FreeLibrary(m_hDLLInstance);
		m_hDLLInstance = NULL;
	}
}