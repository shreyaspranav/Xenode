#pragma once
#include <Core.h>

struct lua_State;

#include "core/scene/Scene.h"

namespace Xen {

	class XEN_API LuaFunctions
	{
	public:

		// Basic Helper functions:
		static void Init();
		static void SetCurrentEntity(const Entity& entity);

		// Funtions related to input:
		static int lua_IsKeyPressed(lua_State* L);

		// Functions to get and set properties of entities:
		static int lua_GetCurrentTransform(lua_State* L);
		static int lua_SetCurrentTransform(lua_State* L);

		// Functions related to physics:
		static int lua_ApplyForceToCentre2D(lua_State* L);

		// Logging Functions:
		static int lua_LogErrorSevere(lua_State* L);
		static int lua_LogError(lua_State* L);
		static int lua_LogWarning(lua_State* L);
		static int lua_LogInfo(lua_State* L);
		static int lua_LogTrace(lua_State* L);

	private:
		static bool IsKeyPressed(char key);
	};
}