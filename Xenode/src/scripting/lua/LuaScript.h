#pragma once
#include "Core.h"

#include "scripting/Script.h"

namespace Xen {
	class XEN_API LuaScript : public Script
	{
	public:
		LuaScript();
		virtual ~LuaScript();

		inline const std::string& GetScriptCode() { return m_ScriptCode; }
	private:
		std::string m_ScriptCode;
	};

}