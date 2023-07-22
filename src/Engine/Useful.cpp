#include "Useful.h"
#include "Core/Logger.h"

#include <Windows.h>

#include <cstdio>
#include <iomanip>
#include <ctime>
#include <filesystem>

//--------------------STRINGS----------------------------

// trim from end of string (right)
std::string& RightTrimString(std::string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
std::string& LeftTrimString(std::string& s, const char* t)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
std::string& TrimString(std::string& s, const char* t)
{
    return LeftTrimString(RightTrimString(s, t), t);
}

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

std::string GetDateTimeString(std::string format)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream dateString;
    dateString << std::put_time(&tm, format.c_str());

    return dateString.str();
}

std::string GetTimeString(std::string format)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    std::ostringstream dateString;
    dateString << std::put_time(&tm, format.c_str());

    return dateString.str();
}

std::string GenerateRandomString(size_t _length)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string tmp_s;
    tmp_s.reserve(_length);
    for (int i = 0; i < _length; ++i)
    {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}

//--------------------FILES AND DIRECTORIES----------------------------

bool PathExists(const std::string& _filePath)
{
    struct stat buffer;
    return (stat(_filePath.c_str(), &buffer) == 0);
}

bool CreateNewDirectory(const std::string& _path)
{
    Logger::LogInformation(FormatString("Created file directory %s", _path.c_str()));
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

std::string OpenFileSelectDialog()
{
    char filename[MAX_PATH];

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = "Text Files\0*.json\0Any File\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Select a File, yo!";
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn))
    {
        return std::string(filename);
    }
    else
    {
        // All this stuff below is to tell you exactly how you messed up above. 
        // Once you've got that fixed, you can often (not always!) reduce it to a 'user cancelled' assumption.
        switch (CommDlgExtendedError())
        {
        case CDERR_DIALOGFAILURE: std::cout << "CDERR_DIALOGFAILURE\n";   break;
        case CDERR_FINDRESFAILURE: std::cout << "CDERR_FINDRESFAILURE\n";  break;
        case CDERR_INITIALIZATION: std::cout << "CDERR_INITIALIZATION\n";  break;
        case CDERR_LOADRESFAILURE: std::cout << "CDERR_LOADRESFAILURE\n";  break;
        case CDERR_LOADSTRFAILURE: std::cout << "CDERR_LOADSTRFAILURE\n";  break;
        case CDERR_LOCKRESFAILURE: std::cout << "CDERR_LOCKRESFAILURE\n";  break;
        case CDERR_MEMALLOCFAILURE: std::cout << "CDERR_MEMALLOCFAILURE\n"; break;
        case CDERR_MEMLOCKFAILURE: std::cout << "CDERR_MEMLOCKFAILURE\n";  break;
        case CDERR_NOHINSTANCE: std::cout << "CDERR_NOHINSTANCE\n";     break;
        case CDERR_NOHOOK: std::cout << "CDERR_NOHOOK\n";          break;
        case CDERR_NOTEMPLATE: std::cout << "CDERR_NOTEMPLATE\n";      break;
        case CDERR_STRUCTSIZE: std::cout << "CDERR_STRUCTSIZE\n";      break;
        case FNERR_BUFFERTOOSMALL: std::cout << "FNERR_BUFFERTOOSMALL\n";  break;
        case FNERR_INVALIDFILENAME: std::cout << "FNERR_INVALIDFILENAME\n"; break;
        case FNERR_SUBCLASSFAILURE: std::cout << "FNERR_SUBCLASSFAILURE\n"; break;
        default: std::cout << "You cancelled.\n";
        }
    }

    return "";
}

std::vector<std::string> GetAllFilesOfType(std::string directory, std::string fileType)
{
    std::string path(directory);
    std::string ext(fileType);

    std::vector<std::string> files;

    for (auto& p : std::filesystem::recursive_directory_iterator(path))
    {
        if (p.path().extension() == ext)
            files.push_back(p.path().string());
    }

    return files;
}

void DeleteFilePath(std::string path)
{
    std::remove(path.c_str());
}

//--------------------ENGINE FILES AND DIRECTORIES----------------------------

std::string GetGameDataFolder()
{
    return GetWorkingDirectory() + "\\GameData";
}

std::string GetFolderLocationRelativeToGameData(std::string path)
{
    path.erase(0, GetWorkingDirectory().length());
    return path;
}

//--------------------NUMBERS----------------------------

float GenerateNumberBetween01()
{
    srand(time(NULL));
    int num = rand() % 100;
    float clampedNum = (float)num / 100.0f;

    return clampedNum;
}
