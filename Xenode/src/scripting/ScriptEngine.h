#pragma once

#include <Core.h>
#include "ScriptLang.h"
#include "Script.h"

namespace Xen {

	class XEN_API ScriptEngine
	{
	public:
		static Ref<ScriptEngine> InitScriptEngine();

		virtual void OnStart(const Ref<Script>& script) = 0;
		virtual void OnUpdate(const Ref<Script>& script, double timestep) = 0;
	};
}

