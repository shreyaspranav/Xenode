#include "pch"
#include "LuaScriptEngine.h"

#include "core/app/Log.h"

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

	void LuaScriptEngine::OnStart(const Ref<Script>& script)
	{
		//LUA_OK		= 0;
		//LUA_YIELD		= 1;
		//LUA_ERRRUN	= 2;
		//LUA_ERRSYNTAX	= 3;
		//LUA_ERRMEM	= 4;
		//LUA_ERRERR	= 5;

		Ref<LuaScript> luaScript = std::dynamic_pointer_cast<LuaScript>(script);

		int status = luaL_dostring(m_LuaVM, luaScript->GetScriptCode().c_str());

		if (status != LUA_OK)
			XEN_ENGINE_LOG_ERROR("Error Occured in script: {0}: Error {1}", script->GetFilePath(), status);

		lua_getglobal(m_LuaVM, luaScript->onStartFunction.c_str());

		if(lua_isfunction(m_LuaVM, -1))
			lua_pcall(m_LuaVM, 0, 1, 0);

		lua_Number n = lua_tonumber(m_LuaVM, -1);

		XEN_ENGINE_LOG_WARN("OnStart Returned from the Lua Script: {0}", (int)n);

		//lua_settop(m_LuaVM, 0);
	}

	void LuaScriptEngine::OnUpdate(const Ref<Script>& script, double timestep)
	{
		Ref<LuaScript> luaScript = std::dynamic_pointer_cast<LuaScript>(script);

		int status = luaL_dostring(m_LuaVM, luaScript->GetScriptCode().c_str());

		if (status != LUA_OK)
			XEN_ENGINE_LOG_ERROR("Error Occured in script: {0}: Error {1}", script->GetFilePath(), status);

		lua_getglobal(m_LuaVM, luaScript->onUpdateFunction.c_str());

		if (lua_isfunction(m_LuaVM, -1))
		{
			lua_pushnumber(m_LuaVM, timestep);
			lua_pcall(m_LuaVM, 1, 1, 0);
		}

		lua_Number n = lua_tonumber(m_LuaVM, -1);

		XEN_ENGINE_LOG_WARN("OnUpdate Returned from the Lua Script: {0}", (double)n);

		//lua_settop(m_LuaVM, 0);
	}
}
