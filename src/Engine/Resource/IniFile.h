#include "Resource.h"

#include <vector>
#include <string>

struct IniFile : public Resource
{
private:

	std::vector<std::string> m_vIniLines;
	std::vector<std::string> GetSettingLine(const std::string& _sectionName, const std::string& _settingName);
	//Returns -1 on not finding the setting in the file.
	int GetIniLineIndex(const std::string& _sectionName, const std::string& _settingName);

	void RewriteIniFile();

public:

	void OnLoad();
	bool GetBooleanSetting(const std::string& _sectionName, const std::string& _settingName, const bool& _defaultValue);
	std::string GetStringSetting(const std::string& _sectionName, const std::string& _settingName, const std::string& _defaultValue);
	int GetIntSetting(const std::string& _sectionName, const std::string& _settingName, const int& _defaultValue);
	float GetFloatSetting(const std::string& _sectionName, const std::string& _settingName, const float& _defaultValue);

	void WriteBooleanSetting(const std::string& _sectionName, const std::string& _settingName, const bool& _value);
	void WriteIntSetting(const std::string& _sectionName, const std::string& _settingName, const int& _value);
	void WriteStringSetting(const std::string& _sectionName, const std::string& _settingName, const std::string& _value);
	void WriteFloatSetting(const std::string& _sectionName, const std::string& _settingName, const float& _value);
};