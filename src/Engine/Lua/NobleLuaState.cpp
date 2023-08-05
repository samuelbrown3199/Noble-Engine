#include "NobleLuaState.h"
#include "../Core/Logger.h"

NobleLuaState::NobleLuaState()
{
	m_luaState.open_libraries(sol::lib::base, sol::lib::package);

	AddFunction("LogInfo", Logger::LogInformation);
	AddFunction("LogError", Logger::LogError);
}

void NobleLuaState::RunScript(std::shared_ptr<Script> script)
{
	m_luaState.script(script->m_sScriptString);
}

void NobleLuaState::RunScriptFunction(std::shared_ptr<Script> script, std::string functionName)
{
	m_luaState.set(script->m_sScriptString);
	sol::function function = m_luaState[functionName];

	function();
}