#include "pch"
#include "LuaFunctions.h"

#include <core/app/Log.h>
#include <lua.hpp>

#include <core/scene/Components.h>

namespace Xen {

	Entity currentEntity;

	void LuaFunctions::SetCurrentEntity(const Entity& entity) { currentEntity = entity; }

	int LuaFunctions::lua_GetCurrentTransform(lua_State* L)
	{
		Component::Transform& transform = currentEntity.GetComponent<Component::Transform>();

		lua_createtable(L, 0, 3);
		{
			lua_pushstring(L, "Position");
			lua_createtable(L, 0, 3);
			{

				lua_pushnumber(L, transform.position.x);
				lua_setfield(L, -2, "x");

				lua_pushnumber(L, transform.position.y);
				lua_setfield(L, -2, "y");

				lua_pushnumber(L, transform.position.z);
				lua_setfield(L, -2, "z");

				lua_settable(L, -3);
			}

			lua_pushstring(L, "Rotation");
			lua_createtable(L, 0, 3);
			{

				lua_pushnumber(L, transform.rotation.x);
				lua_setfield(L, -2, "x");

				lua_pushnumber(L, transform.rotation.y);
				lua_setfield(L, -2, "y");

				lua_pushnumber(L, transform.rotation.z);
				lua_setfield(L, -2, "z");

				lua_settable(L, -3);
			}

			lua_pushstring(L, "Scale");
			lua_createtable(L, 0, 3);
			{

				lua_pushnumber(L, transform.scale.x);
				lua_setfield(L, -2, "x");

				lua_pushnumber(L, transform.scale.y);
				lua_setfield(L, -2, "y");

				lua_pushnumber(L, transform.scale.z);
				lua_setfield(L, -2, "z");

				lua_settable(L, -3);
			}
		}

		return 1;
	}
	int LuaFunctions::lua_LogErrorSevere(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_ERROR_SEVERE(s);

		return 0;
	}
	int LuaFunctions::lua_LogError(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_ERROR(s);

		return 0;
	}
	int LuaFunctions::lua_LogWarning(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_WARN(s);

		return 0;
	}
	int LuaFunctions::lua_LogInfo(lua_State* L)
	{
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_INFO(s);

		return 0;
	}
	int LuaFunctions::lua_LogTrace(lua_State* L)
	{	
		std::string s = lua_tostring(L, 1);
		XEN_APP_LOG_TRACE(s);

		return 0;
	}
}