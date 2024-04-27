#include "Logger.h"
#include "../Useful.h"
#include "Application.h"

void LogConsole(const std::string& _logString)
{
	Application::GetApplication()->GetLogger()->LogConsole(_logString);
}

void LogTrace(const std::string& _logString)
{
	Application::GetApplication()->GetLogger()->LogTrace(_logString);
}

void LogInfo(const std::string& _logString)
{
	Application::GetApplication()->GetLogger()->LogInformation(_logString);
}

void LogError(const std::string& _logString)
{
	Application::GetApplication()->GetLogger()->LogError(_logString);
}

void LogFatalError(const std::string& _logString)
{
	Application::GetApplication()->GetLogger()->LogFatalError(_logString);
}

Logger::Logger()
{
	m_sSessionFileName = GetDateTimeString("%d-%m-%Y");

	logFolder = GetWorkingDirectory() + "\\Logs\\";
	if (!PathExists(logFolder))
		CreateNewDirectory(logFolder);
	else
	{
		std::vector<std::string> files = GetAllFilesOfType(logFolder, ".txt", false);

		for (int i = 0; i < files.size(); i++)
		{
			DeleteFilePath(files.at(i)); //deleting all files currently. Want to delete logs that are a few days old.
		}
	}
}

void Logger::SetLogLevel(std::string logLevel)
{
	if (logLevel == "trace")
		m_logLevel = trace;
	else if (logLevel == "info")
		m_logLevel = info;
	else if (logLevel == "error")
		m_logLevel = error;
	else if (logLevel == "fatal")
		m_logLevel = fatal;
	else if (logLevel == "none")
		m_logLevel = none;
	else
	{
		LogError("Invalid log level passed to SetLogLevel");
		return;
	}
}

void Logger::LogConsole(const std::string& _logString)
{
	std::string passedLine = "Console: ";
	passedLine += _logString;

	LogInformation(passedLine);
	Application::GetApplication()->ConsoleLog(passedLine);
}

void Logger::LogTrace(const std::string& _logString)
{
	if(m_logLevel > trace)
		return;

	std::string passedLine = "Trace: ";
	passedLine += _logString;

	LogInformation(passedLine);
}

void Logger::LogInformation(const std::string& _logString)
{
	if (m_logLevel > info)
		return;

	std::string writtenLine = FormatString("[%s] - ", GetDateTimeString("%H:%M:%S"));
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

void Logger::LogError(const std::string& _logString)
{
	if (m_logLevel > error)
		return;

	std::string passedString = "Error: ";
	passedString += _logString;

	LogInformation(passedString);
}

void Logger::LogFatalError(const std::string& _logString)
{
	if (m_logLevel > fatal)
		return;

	std::string passedString = "Fatal Error: ";
	passedString += _logString;

	LogInformation(passedString);

	throw std::exception(passedString.c_str());
}