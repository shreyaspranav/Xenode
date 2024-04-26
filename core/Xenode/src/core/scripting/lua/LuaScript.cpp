#include "pch"
#include "LuaScript.h"

namespace Xen {
	LuaScript::LuaScript(const std::string& filePath)
		:m_FilePath(filePath)
	{
		std::ifstream stream(filePath);
		std::stringstream ss;

		while (!stream.eof())
		{
			std::string s;
			std::getline(stream, s);

			ss << s << "\n";
		}

		m_ScriptCode = ss.str();
	}
	LuaScript::~LuaScript()
	{
	}
}