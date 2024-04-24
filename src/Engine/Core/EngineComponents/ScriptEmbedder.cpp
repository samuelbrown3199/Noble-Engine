#include "ScriptEmbedder.h"
#include "../Application.h"
#include "../ECS/Entity.h"

void ScriptEmbedder::OnUpdate()
{
	if (m_script == nullptr)
		return;

	m_luaState->RunScript(m_script);
	m_luaState->RunScriptFunction(m_sFunctionName);
}