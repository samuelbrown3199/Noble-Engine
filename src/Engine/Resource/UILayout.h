#include "Resource.h"

#include <vector>

struct UILayout : public Resource
{
	std::vector<std::string> m_vLayoutLines;

	void OnLoad();
};