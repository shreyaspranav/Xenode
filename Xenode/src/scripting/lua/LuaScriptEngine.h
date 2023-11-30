#pragma once

#include <Core.h>
#include <scripting/ScriptEngine.h>

#include "LuaScript.h"
#include "core/scene/Scene.h"

struct lua_State;

namespace Xen {
	class LuaScriptEngine : public ScriptEngine
	{
	public:
		LuaScriptEngine();
		virtual ~LuaScriptEngine();

		void OnStart(const Ref<Script>& script, const Entity& entity) override;
		void OnUpdate(const Ref<Script>& script, const Entity& entity, double timestep) override;

	private:
		void SetupLuaFuntions();
	private:
		lua_State* m_LuaVM;
	};
}