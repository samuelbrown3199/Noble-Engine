#pragma once

#include <iostream>
#include <fstream>

void LogTrace(const std::string& _logString);
void LogInfo(const std::string& _logString);
void LogError(const std::string& _logString);
void LogFatalError(const std::string& _logString);

/*
* Responsible for logging program information for use in debugging.
*/
class Logger
{
	std::ofstream m_logFile;
	std::string m_sSessionFileName;
	std::string logFolder;

public:

	enum LogLevel
	{
		trace,
		info,
		error,
		fatal,
		none
	};
	LogLevel m_logLevel = trace;

	Logger();

	void SetLogLevel(std::string logLevel);

	/*
	* Logs a single line into the session log file.
	*/
	void LogTrace(const std::string& _logString);
	/*
	*Logs a single line into the session log file.
	*/
	void LogInformation(const std::string& _logString);
	/*
	* Logs a non fatal error into the session log file.
	*/
	void LogError(const std::string& _logString);
	/*
	* Logs a fatal error into the session log file.
	*/
	void LogFatalError(const std::string& _logString);
};