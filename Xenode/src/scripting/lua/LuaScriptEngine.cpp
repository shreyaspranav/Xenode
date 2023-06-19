#include "pch"
#include "LuaScriptEngine.h"

#include <lua.hpp>

namespace Xen {
	LuaScriptEngine::LuaScriptEngine()
	{
		m_LuaVM = luaL_newstate();
	}
	
	LuaScriptEngine::~LuaScriptEngine()
	{
		lua_close(m_LuaVM);
	}
}
