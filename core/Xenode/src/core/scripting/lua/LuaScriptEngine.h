#pragma once

#include <Core.h>
#include <core/scripting/ScriptEngine.h>

#include "LuaScript.h"
#include <core/scene/Scene.h>

struct lua_State;

namespace Xen {
	class LuaScriptEngine : public ScriptEngine
	{
	public:
		LuaScriptEngine();
		virtual ~LuaScriptEngine();

		void OnStart(const Ref<Script>& script, Entity entity) override;
		void OnUpdate(const Ref<Script>& script, Entity entity, double timestep) override;
		void OnFixedUpdate(const Ref<Script>& script, Entity entity) override;

	private:
		void SetupLuaFunctions();
		void AddLuaPath(const std::string& path);
	private:
		lua_State* m_LuaVM;
	};
}