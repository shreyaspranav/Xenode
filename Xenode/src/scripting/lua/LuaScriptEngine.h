#pragma once

#include <Core.h>
#include <scripting/ScriptEngine.h>

struct lua_State;

namespace Xen {
	class LuaScriptEngine : public ScriptEngine
	{
	public:
		LuaScriptEngine();
		virtual ~LuaScriptEngine();
	private:
		lua_State* m_LuaVM;

	};
}