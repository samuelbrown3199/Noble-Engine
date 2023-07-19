#include "Logger.h"
#include "../Useful.h"

std::ofstream Logger::m_logFile;
std::string Logger::m_sSessionFileName;
bool Logger::m_bUseLogging;

Logger::Logger()
{
	m_sSessionFileName = GetDateTimeString();
}

void Logger::LogInformation(const std::string& _logString)
{
	if (!m_bUseLogging)
		return;

	std::string writtenLine = FormatString("[%s] - ", GetTimeString());
	writtenLine += _logString + "\n";
	
	std::cout << writtenLine;
	std::string logFolder = GetWorkingDirectory() + "\\Logs\\";
	if (!PathExists(logFolder))
		CreateNewDirectory(logFolder);

	std::string fileName = logFolder + m_sSessionFileName+".txt";
	m_logFile.open(fileName, std::ios::app);
	if (m_logFile.is_open())
	{
		m_logFile.write(writtenLine.c_str(), sizeof(char) * writtenLine.length());
		m_logFile.close();
		return;
	}
	std::cout << "Failed to open log file>: " << m_sSessionFileName << std::endl;
}

void Logger::LogError(const std::string& _logString, const int& _errorLevel)
{
	if (!m_bUseLogging && _errorLevel != 2)
		return;

	std::string passedString;

	switch (_errorLevel)
	{
	case 0:
		passedString = "Minor Error: ";
		break;
	case 1:
		passedString = "Major Error: ";
		break;
	case 2:
		passedString = "Fatal Error: ";
		break;
	default:
		passedString = "Unknown error: ";
		break;
	}

	passedString += _logString+"\n";

	std::cout << passedString;
	LogInformation(passedString);

	if (_errorLevel == 2)
		throw std::exception(passedString.c_str());
}