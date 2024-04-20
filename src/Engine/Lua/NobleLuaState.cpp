#include "NobleLuaState.h"

#include "../Core/Application.h"
#include "../Core/Graphics/Renderer.h"
#include "../Core/Logger.h"

NobleLuaState::NobleLuaState()
{
	m_luaState.open_libraries(sol::lib::base, sol::lib::package);
	Renderer* renderer = Application::GetApplication()->GetRenderer();

	//AddFunction("SetClearColour", renderer->SetClearColour);

	AddFunction("LogInfo", LogInfo);
	AddFunction("LogError", LogError);
}

void NobleLuaState::SetScript(std::shared_ptr<Script> script)
{
	m_luaState.script(script->m_sScriptString);
}

void NobleLuaState::RunScript(std::shared_ptr<Script> script)
{
	m_luaState.script(script->m_sScriptString);
}

void NobleLuaState::RunScriptFunction(std::string functionName)
{
	sol::function function = m_luaState[functionName];
	function();
}