#include "IniFile.h"
#include "../../Core/Logger.h"
#include "../../Useful.h"

#include <fstream>

IniFile::IniFile(std::string path)
{
	m_sIniFilePath = path;
	OnLoad();
}

void IniFile::OnLoad()
{
	std::ifstream iniFile;
	iniFile.open(m_sIniFilePath);
	std::string line;
	while (std::getline(iniFile, line))
	{
		m_vIniLines.push_back(line);
	}
	iniFile.close();
}

std::vector<std::string> IniFile::GetSettingLine(const std::string& _sectionName, const std::string& _settingName)
{
	std::string currentSection = "";
	std::vector<std::string> splitSetting;

	for (int i = 0; i < m_vIniLines.size(); i++)
	{
		std::string currentLine = m_vIniLines.at(i);
		if (currentLine.find("[") != std::string::npos)
		{
			currentSection = currentLine;
		}
		if (currentSection != FormatString("[%s]", _sectionName))
			continue;

		splitSetting = SplitString(currentLine, '=');
		if (splitSetting.size() == 2)
		{
			std::string trim = " ";
			TrimString(splitSetting.at(0), trim.c_str());
			TrimString(splitSetting.at(1), trim.c_str());
			if (splitSetting.at(0) == _settingName)
				break;
		}
	}

	return splitSetting;
}

bool IniFile::GetBooleanSetting(const std::string& _sectionName, const std::string& _settingName, const bool& _defaultValue)
{
	bool settingValue = _defaultValue;
	int integerValue = GetIntSetting(_sectionName, _settingName, _defaultValue);
	if (integerValue > 1 || integerValue < 0)
	{
		Logger::LogError(FormatString("Trying to load boolean [%s] %s as an invalid value %d", _sectionName.c_str(), _settingName.c_str(), integerValue), 0);
		return settingValue;
	}

	settingValue = integerValue == 1 ? true : false;
	return settingValue;
}

std::string IniFile::GetStringSetting(const std::string& _sectionName, const std::string& _settingName, const std::string& _defaultValue)
{
	std::string settingValue = _defaultValue;
	std::vector<std::string> splitSetting = GetSettingLine(_sectionName, _settingName);
	if (splitSetting.size() == 2)
		settingValue = splitSetting.at(1);

	Logger::LogInformation(FormatString("Loading [%s] %s as %s", _sectionName.c_str(), _settingName.c_str(), settingValue.c_str()));
	return settingValue;
}

int IniFile::GetIntSetting(const std::string& _sectionName, const std::string& _settingName, const int& _defaultValue)
{
	int settingValue = _defaultValue;
	std::vector<std::string> splitSetting = GetSettingLine(_sectionName, _settingName);
	if (splitSetting.size() == 2)
		settingValue = std::stoi(splitSetting.at(1));

	Logger::LogInformation(FormatString("Loading [%s] %s as %d", _sectionName.c_str(), _settingName.c_str(), settingValue));
	return settingValue;
}

float IniFile::GetFloatSetting(const std::string& _sectionName, const std::string& _settingName, const float& _defaultValue)
{
	float settingValue = _defaultValue;
	std::vector<std::string> splitSetting = GetSettingLine(_sectionName, _settingName);
	if (splitSetting.size() == 2)
		settingValue = std::stof(splitSetting.at(1));

	Logger::LogInformation(FormatString("Loading [%s] %s as %1f", _sectionName.c_str(), _settingName.c_str(), settingValue));
	return settingValue;
}

int IniFile::GetIniLineIndex(const std::string& _sectionName, const std::string& _settingName)
{
	std::string currentSection = "";
	std::vector<std::string> splitSetting;
	for (int i = 0; i < m_vIniLines.size(); i++)
	{
		std::string currentLine = m_vIniLines.at(i);
		if (currentLine.find("[") != std::string::npos)
		{
			currentSection = currentLine;
		}
		if (currentSection != FormatString("[%s]", _sectionName))
			continue;

		splitSetting = SplitString(currentLine, '=');
		if (splitSetting.at(0) == _settingName)
		{
			return i;
		}
	}

	return -1;
}

void IniFile::RewriteIniFile()
{
	std::ofstream iniFile;
	iniFile.open(m_sIniFilePath, std::ofstream::out, std::ofstream::trunc);
	for (int i = 0; i < m_vIniLines.size(); i++)
	{
		std::string newLine = m_vIniLines.at(i) + "\n";
		iniFile.write(newLine.c_str(), sizeof(char) * newLine.length());
	}
	iniFile.close();
}

void IniFile::WriteBooleanSetting(const std::string& _sectionName, const std::string& _settingName, const bool& _value)
{
	if (_value > 1 || _value < 0)
	{
		Logger::LogError(FormatString("Trying to change boolean %s %s to invalid value %d", _sectionName.c_str(), _settingName.c_str(), _value), 0);
		return;
	}

	WriteIntSetting(_sectionName, _settingName, _value);
}
void IniFile::WriteIntSetting(const std::string& _sectionName, const std::string& _settingName, const int& _value)
{
	Logger::LogInformation(FormatString("Updating %s %s to value %d", _sectionName.c_str(), _settingName.c_str(), _value));
	int lineIndex = GetIniLineIndex(_sectionName, _settingName);
	if (lineIndex == -1)
	{
		Logger::LogError(FormatString("Trying to change value %s %s which does not exist in file.", _sectionName.c_str(), _settingName.c_str()), 0);
		return;
	}

	m_vIniLines.at(lineIndex) = FormatString("%s=%d", _settingName.c_str(), _value);
	RewriteIniFile();
}

void IniFile::WriteStringSetting(const std::string& _sectionName, const std::string& _settingName, const std::string& _value)
{
	Logger::LogInformation(FormatString("Updating %s %s to value %s", _sectionName.c_str(), _settingName.c_str(), _value.c_str()));
	int lineIndex = GetIniLineIndex(_sectionName, _settingName);
	if (lineIndex == -1)
	{
		Logger::LogError(FormatString("Trying to change value %s %s which does not exist in file.", _sectionName.c_str(), _settingName.c_str()), 0);
		return;
	}

	m_vIniLines.at(lineIndex) = FormatString("%s=%s", _settingName.c_str(), _value);
	RewriteIniFile();
}

void IniFile::WriteFloatSetting(const std::string& _sectionName, const std::string& _settingName, const float& _value)
{
	Logger::LogInformation(FormatString("Updating %s %s to value %1f", _sectionName.c_str(), _settingName.c_str(), _value));
	int lineIndex = GetIniLineIndex(_sectionName, _settingName);
	if (lineIndex == -1)
	{
		Logger::LogError(FormatString("Trying to change value %s %s which does not exist in file.", _sectionName.c_str(), _settingName.c_str()), 0);
		return;
	}

	m_vIniLines.at(lineIndex) = FormatString("%s=%1f", _settingName.c_str(), _value);
	RewriteIniFile();
}