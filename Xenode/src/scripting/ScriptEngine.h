#pragma once

#include <Core.h>
#include "ScriptLang.h"
#include "Script.h"

namespace Xen {

	class Entity;

	class XEN_API ScriptEngine
	{
	public:
		static Ref<ScriptEngine> InitScriptEngine();

		virtual void OnStart(const Ref<Script>& script, const Entity& entity) = 0;
		virtual void OnUpdate(const Ref<Script>& script, const Entity& entity, double timestep) = 0;
	};
}

