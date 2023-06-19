#pragma once

#include <Core.h>
#include "ScriptLang.h"
#include "Script.h"

namespace Xen {

	class XEN_API ScriptEngine
	{
	public:
		static Ref<ScriptEngine> InitScriptEngine();

		virtual void AddScript(Ref<Script> script) = 0;

		virtual void OnSetup() = 0;
	};
}

