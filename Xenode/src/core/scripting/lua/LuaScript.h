#pragma once
#include "Core.h"

#include "core/scripting/Script.h"

namespace Xen {
	class XEN_API LuaScript : public Script
	{
	public:
		std::string 
			onStartFunction			= "OnStart",
			onUpdateFunction		= "OnUpdate",
			onFixedUpdateFunction	= "OnFixedUpdate";
	public:
		LuaScript(const std::string& filePath);
		virtual ~LuaScript();

		inline const std::string& GetScriptCode() { return m_ScriptCode; }

		inline const std::string& GetFilePath() const override { return m_FilePath; };
	private:
		std::string m_ScriptCode;
		std::string m_FilePath;
	};

}