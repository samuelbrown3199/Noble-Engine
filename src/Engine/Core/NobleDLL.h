#pragma once

#include <string>
#include <memory>
#include <map>
#include <Windows.h>

class Application;

class NobleDLL
{
protected:

	HINSTANCE m_hDLLInstance;
	std::string m_sDLLPath;

public:

	NobleDLL(std::string path);

	void LoadDLL(std::shared_ptr<Application> application);
	void UnloadDLL();
};