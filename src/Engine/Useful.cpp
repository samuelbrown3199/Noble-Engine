#include "Useful.h"
#include "Core/Logger.h"
#include "Core/ResourceManager.h"

#include "Core/VersionInformation.h"

#include <Windows.h>

#include <cstdio>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <chrono>

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
    auto now = std::chrono::system_clock::now();

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    auto timer = std::chrono::system_clock::to_time_t(now);
    std::tm bt = *std::localtime(&timer);

    std::ostringstream dateString;
    dateString << std::put_time(&bt, format.c_str());

    if(format.find("%S") != std::string::npos)
        dateString << '.' << std::setfill('0') << std::setw(3) << ms.count();

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

std::string GetVersionInfoString()
{
    std::string versionString = FormatString("%s-%d.%d.%d", VERSION_STAGE, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    return versionString;
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

std::string OpenFileSelectDialog(std::string filter)
{
    char filename[MAX_PATH];

    std::string filterString = FormatString("Text Files\0*%s\0Any File\0*.*\0", filter);

    OPENFILENAME ofn;
    ZeroMemory(&filename, sizeof(filename));
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
    ofn.lpstrFilter = filterString.c_str();
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = "Select a File.";
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

void AddVersionDataToJson(nlohmann::json& data)
{
    data["EngineVersion"] = GetVersionInfoString();
}

//--------------------ENGINE FILES AND DIRECTORIES----------------------------

std::string GetGameFolder()
{
    return ResourceManager::GetResourceManagerWorkingDirectory();
}

std::string GetGameDataFolder()
{
    return GetGameFolder() + "\\GameData";
}

std::string GetFolderLocationRelativeToGameData(std::string path)
{
    path.erase(0, ResourceManager::GetResourceManagerWorkingDirectory().length());
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

//-------------------MATHS-------------------------------

bool IsPointInViewFrustum(const glm::vec3& point, const glm::mat4& VP)
{
    glm::vec4 v4Point(point, 1.0f);
    glm::vec4 clipSpacePoint = VP * v4Point;


    bool inViewFrustum = ((clipSpacePoint.x <= clipSpacePoint.w) &&
        (clipSpacePoint.x >= -clipSpacePoint.w) &&
        (clipSpacePoint.y <= clipSpacePoint.w) &&
        (clipSpacePoint.y >= -clipSpacePoint.w) &&
        (clipSpacePoint.z <= clipSpacePoint.w) &&
        (clipSpacePoint.z >= -clipSpacePoint.w));

    return inViewFrustum;
}