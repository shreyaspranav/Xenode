#pragma once

#include <Core.h>
#include "ScriptLang.h"

namespace Xen {

	class XEN_API ScriptEngine
	{
		static Ref<ScriptEngine> InitScriptEngine(ScriptLang lang);
	};
}

