#pragma once
#include <Core.h>

struct lua_State;

namespace Xen {

	class XEN_API LuaFunctions
	{
	public:

		// Logging Functions:
		static int lua_LogErrorSevere(lua_State* L);
		static int lua_LogError(lua_State* L);
		static int lua_LogWarning(lua_State* L);
		static int lua_LogInfo(lua_State* L);
		static int lua_LogTrace(lua_State* L);
	};
}