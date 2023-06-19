#pragma once
#include <Core.h>

#include "ScriptLang.h"

namespace Xen {
	class XEN_API Script
	{
		static Ref<Script> CreateScript(ScriptLang lang);
	};
}
