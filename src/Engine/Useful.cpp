#include "Useful.h"
#include "Core/Logger.h"

#include <iomanip>
#include <ctime>
#include <sstream>

//--------------------STRINGS----------------------------

std::vector<std::string> SplitString(const std::string& _input, const char _delimiter)
{
    std::vector<std::string> strings;
    std::istringstream f{ _input };
    std::string s;
    while (std::getline(f, s, _delimiter))
    {
        strings.push_back(s);
    }

    return strings;
}

std::string RemoveCharacterFromString(const std::string& _input, const char _targetCharacter)
{
    std::string returnString = _input;
    returnString.erase(std::remove(returnString.begin(), returnString.end(), _targetCharacter), returnString.end());

    return returnString;
}

std::string GetDateTimeString()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream dateString;
    dateString << std::put_time(&tm, "%d-%m-%Y-%H-%M-%S");

    return dateString.str();
}

std::string GetTimeString()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream dateString;
    dateString << std::put_time(&tm, "%H:%M:%S");

    return dateString.str();
}

//--------------------FILES AND DIRECTORIES----------------------------

bool PathExists(const std::string& _filePath)
{
    struct stat buffer;
    return (stat(_filePath.c_str(), &buffer) == 0);
}

bool CreateDirectory(const std::string& _path)
{
    //Logger::LogInformation(FormatString("Created file directory %s", _path.c_str()));
    return (_mkdir(_path.c_str()) == 0);
}

bool DeleteDirectory(const std::string& _path)
{
    Logger::LogInformation(FormatString("Deleted file directory %s", _path.c_str()));
    return (_rmdir(_path.c_str()) == 0);
}

std::string GetWorkingDirectory()
{
    char temp[256];
    return (getcwd(temp, sizeof(temp)) ? std::string(temp) : std::string(""));
}

//--------------------NUMBERS----------------------------

float GenerateNumberBetween01()
{
    srand(time(NULL));
    int num = rand() % 100;
    float clampedNum = (float)num / 100.0f;

    return clampedNum;
}
