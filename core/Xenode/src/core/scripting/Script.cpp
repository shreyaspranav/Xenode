#include "pch"
#include "Script.h"

#include <core/app/GameApplication.h>

#include <core/scripting/lua/LuaScript.h>

#include <core/app/Log.h>

namespace Xen {
	Ref<Script> Script::CreateScript(const std::string& filePath)
	{
		switch (GetApplicationInstance()->GetScriptLang())
		{
		case ScriptLang::Lua:
			return std::make_shared<LuaScript>(filePath);
			break;
		default:
			break;
		}

		XEN_ENGINE_LOG_ERROR("Unknown Scripting Language!");
		TRIGGER_BREAKPOINT;

		return nullptr;
	}

}