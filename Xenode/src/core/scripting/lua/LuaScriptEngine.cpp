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
		LuaFunctions::Init();
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
		{
			XEN_ENGINE_LOG_ERROR("Error Occured in script: {0}: Error {1}", script->GetFilePath(), lua_tostring(m_LuaVM, -1));
			lua_pop(m_LuaVM, 1);
		}

		// Make sure to change this when asset system is implemented 
		// AddLuaPath("assets/scripts/");

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
		{
			XEN_ENGINE_LOG_ERROR("Error Occured in script: {0}: Error {1}", script->GetFilePath(), lua_tostring(m_LuaVM, -1));
			lua_pop(m_LuaVM, 1);
		}

		// Make sure to change this when asset system is implemented 
		// AddLuaPath("assets/scripts/");

		lua_getglobal(m_LuaVM, luaScript->onUpdateFunction.c_str());

		if (lua_isfunction(m_LuaVM, -1)) {
			lua_pushnumber(m_LuaVM, timestep);
			lua_pcall(m_LuaVM, 1, 0, 0);
		}
	}

	void LuaScriptEngine::SetupLuaFuntions()
	{
		// Funtions related to input: -----------------------------------------------------------
		lua_register(m_LuaVM, "IsKeyPressed", LuaFunctions::lua_IsKeyPressed);
		lua_register(m_LuaVM, "IsMouseButtonPressed", LuaFunctions::lua_IsMouseButtonPressed);
		lua_register(m_LuaVM, "GetNormalizedMouseCoords2D", LuaFunctions::lua_GetNormalizedMouseCoords2D);

		// Functions related to physics: --------------------------------------------------------
		lua_register(m_LuaVM, "ApplyForceToCentre2D", LuaFunctions::lua_ApplyForceToCentre2D);
		lua_register(m_LuaVM, "ApplyForce2D", LuaFunctions::lua_ApplyForce2D);

		// Functions to get and set the transform: ----------------------------------------------
		lua_register(m_LuaVM, "GetCurrentTransform", LuaFunctions::lua_GetCurrentTransform);
		lua_register(m_LuaVM, "SetCurrentTransform", LuaFunctions::lua_SetCurrentTransform);

		// Logging functions: -------------------------------------------------------------------
		lua_register(m_LuaVM, "LogErrorSevere", LuaFunctions::lua_LogErrorSevere);
		lua_register(m_LuaVM, "LogError", LuaFunctions::lua_LogError);
		lua_register(m_LuaVM, "LogWarning", LuaFunctions::lua_LogWarning);
		lua_register(m_LuaVM, "LogInfo", LuaFunctions::lua_LogInfo);
		lua_register(m_LuaVM, "LogTrace", LuaFunctions::lua_LogTrace);
	}

	void LuaScriptEngine::AddLuaPath(const std::string& path)
	{
		lua_getglobal(m_LuaVM, "package");
		lua_getfield(m_LuaVM, -1, "path");

		std::string currentPath = lua_tostring(m_LuaVM, -1);
		currentPath.append(";");
		currentPath.append(path);

		lua_pop(m_LuaVM, 1);

		lua_pushstring(m_LuaVM, currentPath.c_str());
		lua_setfield(m_LuaVM, -2, "path");
		lua_pop(m_LuaVM, 1);
	}
}
