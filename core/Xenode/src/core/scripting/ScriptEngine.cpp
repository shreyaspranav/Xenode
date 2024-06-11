#include "pch"
#include "ScriptEngine.h"

#include "core/app/Log.h"
#include "core/app/DesktopApplication.h"

#include "lua/LuaScriptEngine.h"

namespace Xen {
	ScriptEngine* ScriptEngine::InitScriptEngine() 
	{
		switch (DesktopApplication::GetScriptingLanguage())
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