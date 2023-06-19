#include "pch"
#include "ScriptEngine.h"

#include "core/app/Log.h"

#include "lua/LuaScriptEngine.h"

namespace Xen {
	Ref<ScriptEngine> ScriptEngine::InitScriptEngine(ScriptLang lang) 
	{
		switch (lang)
		{
		case ScriptLang::Lua:
			return std::make_shared<LuaScriptEngine>();
			break;
		}

		XEN_ENGINE_LOG_ERROR("Unknown Scripting Language!");
		TRIGGER_BREAKPOINT;
	}
}