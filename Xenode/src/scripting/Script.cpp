#include "pch"
#include "Script.h"

#include "core/app/Log.h"

namespace Xen {
	Ref<Script> CreateScript(ScriptLang lang)
	{
		switch (lang)
		{
		case Lua:

			break;
		}

		XEN_ENGINE_LOG_ERROR("Unknown Scripting Language!");
		TRIGGER_BREAKPOINT;
	}

}