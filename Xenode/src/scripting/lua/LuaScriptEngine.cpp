#include "pch"
#include "LuaScriptEngine.h"

#include "core/app/Log.h"

#include <lua.hpp>

#include "LuaFunctions.h"

namespace Xen {
	LuaScriptEngine::LuaScriptEngine()
	{
		m_LuaVM = luaL_newstate();

		SetupLuaFuntions();
	}
	
	LuaScriptEngine::~LuaScriptEngine()
	{
		lua_close(m_LuaVM);
	}

	void LuaScriptEngine::OnStart(const Ref<Script>& script, const Entity& entity)
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
			lua_pcall(m_LuaVM, 0, 0, 0);
	}

	void LuaScriptEngine::OnUpdate(const Ref<Script>& script, const Entity& entity, double timestep)
	{
		Ref<LuaScript> luaScript = std::dynamic_pointer_cast<LuaScript>(script);

		LuaFunctions::SetCurrentEntity(entity);

		int status = luaL_dostring(m_LuaVM, luaScript->GetScriptCode().c_str());
		
		if (status != LUA_OK)
			XEN_ENGINE_LOG_ERROR("Error Occured in script: {0}: Error {1}", script->GetFilePath(), status);

		lua_getglobal(m_LuaVM, luaScript->onUpdateFunction.c_str());

		if (lua_isfunction(m_LuaVM, -1)) {
			lua_pushnumber(m_LuaVM, timestep);
			lua_pcall(m_LuaVM, 1, 0, 0);
		}
	}

	void LuaScriptEngine::SetupLuaFuntions()
	{
		lua_register(m_LuaVM, "GetCurrentTransform", LuaFunctions::lua_GetCurrentTransform);
		lua_register(m_LuaVM, "SetCurrentTransform", LuaFunctions::lua_SetCurrentTransform);

		// Logging functions: -------------------------------------------------------------------
		lua_register(m_LuaVM, "LogErrorSevere", LuaFunctions::lua_LogErrorSevere);
		lua_register(m_LuaVM, "LogError", LuaFunctions::lua_LogError);
		lua_register(m_LuaVM, "LogWarning", LuaFunctions::lua_LogWarning);
		lua_register(m_LuaVM, "LogInfo", LuaFunctions::lua_LogInfo);
		lua_register(m_LuaVM, "LogTrace", LuaFunctions::lua_LogTrace);
	}
}
