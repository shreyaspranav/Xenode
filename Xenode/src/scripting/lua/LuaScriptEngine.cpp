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
	void LuaScriptEngine::AddScript(Ref<Script> script)
	{
		Ref<LuaScript> luaScript = std::dynamic_pointer_cast<LuaScript>(script);
		m_Scripts.push_back(luaScript);
	}

	void LuaScriptEngine::OnSetup()
	{

		const int OK		= 0;
		const int YIELD		= 1;
		const int ERRRUN	= 2;
		const int ERRSYNTAX	= 3;
		const int ERRMEM	= 4;
		const int ERRERR	= 5;

		for (const Ref<LuaScript>& script : m_Scripts)
		{
			int status = luaL_dostring(m_LuaVM, script->GetScriptCode().c_str());

			if (status != LUA_OK)
			{
				XEN_ENGINE_LOG_ERROR("Error Occured in script: {0}: Error {1}", script->GetFilePath(), status);
				continue;
			}
			lua_getglobal(m_LuaVM, "OnSetup");

			if(lua_isfunction(m_LuaVM, -1))
				lua_pcall(m_LuaVM, 0, 1, 0);

			lua_Number n = lua_tonumber(m_LuaVM, -1);

			XEN_ENGINE_LOG_WARN("OnSetup Returned from the Lua Script: {0}", (int)n);

			lua_settop(m_LuaVM, 0);
		}
	}
}
