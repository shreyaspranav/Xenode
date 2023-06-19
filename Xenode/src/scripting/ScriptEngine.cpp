#include "pch"
#include "ScriptEngine.h"

#include "core/app/Log.h"
#include "core/app/DesktopApplication.h"

#include "lua/LuaScriptEngine.h"

namespace Xen {
	Ref<ScriptEngine> ScriptEngine::InitScriptEngine() 
	{
		switch (DesktopApplication::GetScriptingLanguage())
		{
		case ScriptLang::Lua:
			return std::make_shared<LuaScriptEngine>();
			break;
		}

		XEN_ENGINE_LOG_ERROR("Unknown Scripting Language!");
		TRIGGER_BREAKPOINT;
	}
}