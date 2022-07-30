#pragma once
#ifndef USEFUL_H_
#define USEFUL_H_

#include <memory>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

//--------------------STRINGS----------------------------

template<typename ... Args>
std::string FormatString(const std::string& _format, Args ... _args)
{
    int size_s = std::snprintf(nullptr, 0, _format.c_str(), _args ...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, _format.c_str(), _args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

std::vector<std::string> SplitString(const std::string& _input, const char _delimiter);
std::string RemoveCharacterFromString(const std::string& _input, const char _targetCharacter);
std::string GetDateTimeString();
std::string GetTimeString();

//--------------------FILES AND DIRECTORIES----------------------------

bool PathExists(const std::string& _path);
bool CreateDirectory(const std::string& _path);
bool DeleteDirectory(const std::string& _path);
std::string GetWorkingDirectory();


//--------------------NUMBERS----------------------------

float GenerateNumberBetween01();

#endif