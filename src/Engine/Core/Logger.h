#pragma once
#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>

/*
* Responsible for logging program information for use in debugging.
*/
class Logger
{
static std::ofstream m_logFile;
static std::string m_sSessionFileName;
static std::string logFolder;

public:
	static bool m_bUseLogging;
	Logger();
	/*
	*Logs a single line into the session log file.
	*/
	static void LogInformation(const std::string& _logString);
	/*
	* Logs an engine error. Different levels : 0 Minor Error, 1 Major Error , 2 Fatal Error
	*/
	static void LogError(const std::string& _logString, const int& _errorLevel);
};

#endif