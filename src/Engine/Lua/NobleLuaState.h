#pragma once

#define SOL_ALL_SAFETIES_ON 1
#include "sol.hpp"

#include "../Resource/Script.h"

#include <memory>

class NobleLuaState
{
private:

	sol::state m_luaState;

public:

	NobleLuaState();

	template <typename ... Args>
	void AddFunction(std::string funcName, Args&&... args)
	{
		m_luaState.set_function(funcName, std::forward<Args>(args)...);
	}

	void SetScript(std::shared_ptr<Script> script);
	void RunScript(std::shared_ptr<Script> script);
	void RunScriptFunction(std::string functionName);

	template <typename ... Args>
	void RunScriptFunction(std::string functionName, Args&&... args)
	{
		sol::function function = m_luaState[functionName];
		function(std::forward<Args>(args)...);
	}

	template<typename T>
	T GetVariable(std::string varName)
	{
		return m_luaState[varName];
	};

	template<typename T>
	void SetVariable(std::string varName, T varValue)
	{
		m_luaState[varName] = varValue;
	}
};