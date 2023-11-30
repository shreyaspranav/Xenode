#pragma once
#include <Core.h>

struct lua_State;

#include "core/scene/Scene.h"

namespace Xen {

	class XEN_API LuaFunctions
	{
	public:

		static void SetCurrentEntity(const Entity& entity);

		// Function to get properties of entities:
		static int lua_GetCurrentTransform(lua_State* L);

		// Logging Functions:
		static int lua_LogErrorSevere(lua_State* L);
		static int lua_LogError(lua_State* L);
		static int lua_LogWarning(lua_State* L);
		static int lua_LogInfo(lua_State* L);
		static int lua_LogTrace(lua_State* L);
	};
}