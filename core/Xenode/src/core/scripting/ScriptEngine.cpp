#include "pch"
#include "ScriptEngine.h"

#include <core/app/Log.h>
#include <core/app/GameApplication.h>

#include "lua/LuaScriptEngine.h"

namespace Xen {
	ScriptEngine* ScriptEngine::InitScriptEngine() 
	{
		switch (GetApplicationInstance()->GetScriptLang())
		{
		case ScriptLang::Lua:
			return new LuaScriptEngine();
			break;
		}

		XEN_ENGINE_LOG_ERROR("Unknown Scripting Language!");
		TRIGGER_BREAKPOINT;

		return nullptr;
	}
}