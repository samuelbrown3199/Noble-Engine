#include "Logger.h"
#include "../Useful.h"

std::ofstream Logger::m_logFile;
std::string Logger::m_sSessionFileName;
std::string Logger::logFolder;

bool Logger::m_bUseLogging;

Logger::Logger()
{
	m_sSessionFileName = GetDateTimeString("%d-%m-%Y");

	logFolder = GetWorkingDirectory() + "\\Logs\\";
	if (!PathExists(logFolder))
		CreateNewDirectory(logFolder);
	else
	{
		std::vector<std::string> files = GetAllFilesOfType(logFolder, ".txt");

		for (int i = 0; i < files.size(); i++)
		{
			DeleteFilePath(files.at(i)); //deleting all files currently. Want to delete logs that are a few days old.
		}
	}
}

void Logger::LogInformation(const std::string& _logString)
{
	if (!m_bUseLogging)
		return;

	std::string writtenLine = FormatString("[%s] - ", GetTimeString("%H:%M:%S"));
	writtenLine += _logString + "\n";
	
	std::cout << writtenLine;

	std::string fileName = logFolder + m_sSessionFileName+".txt";
	m_logFile.open(fileName, std::ios::app);
	if (m_logFile.is_open())
	{
		m_logFile.write(writtenLine.c_str(), sizeof(char) * writtenLine.length());
		m_logFile.close();
		return;
	}

	throw std::exception(FormatString("Failed to open log file>: %s", m_sSessionFileName).c_str());
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

	LogInformation(passedString);

	if (_errorLevel == 2)
		throw std::exception(passedString.c_str());
}