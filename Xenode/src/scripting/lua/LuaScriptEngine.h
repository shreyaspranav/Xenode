#pragma once

#include <Core.h>
#include <scripting/ScriptEngine.h>

#include "LuaScript.h"

struct lua_State;

namespace Xen {
	class LuaScriptEngine : public ScriptEngine
	{
	public:
		LuaScriptEngine();
		virtual ~LuaScriptEngine();

		void AddScript(Ref<Script> script) override;

		void OnStart() override;
		void OnUpdate(double timestep) override;
	private:
		lua_State* m_LuaVM;
		std::vector<Ref<LuaScript>> m_Scripts;
	};
}