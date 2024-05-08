#pragma once
#include <Core.h>

#include <core/app/Log.h>
#include <core/scene/Scene.h>

namespace Xen 
{
	enum class GameType : int8_t { _2D, _3D };
	enum class ScriptingLanguage : int8_t { Lua, CSharp };

	// Struct to represent the project properties. It is required during the creation of a project:
	struct ProjectProperties
	{
		std::string name = "UnnamedProject";
		GameType gameType = GameType::_2D;
		ScriptingLanguage scriptingLanguage = ScriptingLanguage::Lua;

	};

	// Struct to represent the project settings that can be changed after the creation of the project:
	struct ProjectSettings
	{
		std::filesystem::path relCacheDirectory = ".projcache/";
		std::filesystem::path relAssetDirectory = "assets/";
		std::filesystem::path relBuildDirectory = "build/";

		std::filesystem::path relStartScenePath = "assets/Scene.xen";
		Ref<Scene> startScene;
	};


	class ProjectUtil
	{
	public:
		static std::string ToGameTypeString(GameType type)
		{
			switch (type)
			{
			case GameType::_2D:	return "2D";
			case GameType::_3D:	return "3D";
			}

			XEN_ENGINE_LOG_ERROR("Unknown GameType!");
			TRIGGER_BREAKPOINT;
		}

		static GameType ToGameTypeFromString(const std::string& gameType)
		{
			if (gameType == "2D")
				return GameType::_2D;
			else if (gameType == "3D")
				return GameType::_3D;
			else 
			{
				XEN_ENGINE_LOG_ERROR("Unknown GameType!");
				TRIGGER_BREAKPOINT;
			}
		}

		static std::string ToScriptingLanguageString(ScriptingLanguage lang)
		{
			switch (lang)
			{
			case ScriptingLanguage::Lua:	return "Lua";
			case ScriptingLanguage::CSharp:	return "C#";
			}

			XEN_ENGINE_LOG_ERROR("Unknown Scripting Language!");
			TRIGGER_BREAKPOINT;
		}

		static ScriptingLanguage ToScriptingLanguageFromString(const std::string& lang)
		{
			if (lang == "Lua")
				return ScriptingLanguage::Lua;
			else if (lang == "C#")
				return ScriptingLanguage::CSharp;
			else
			{
				XEN_ENGINE_LOG_ERROR("Unknown GameType!");
				TRIGGER_BREAKPOINT;
			}
		}
	};

	class XEN_API Project
	{
	public:
		Project() = delete;

		Project(const ProjectProperties& properties)
			:m_ProjectProperties(properties) 
		{
			m_ProjectSettings.startScene = std::make_shared<Scene>();
			m_ProjectSettings.startScene->NewScene();
		}

		~Project() {}

		static Ref<Project> CreateProject(const ProjectProperties& properties)
		{
			return std::make_shared<Project>(properties);
		}

		ProjectProperties& GetProjectProperties()	{ return m_ProjectProperties; }
		ProjectSettings& GetProjectSettings()		{ return m_ProjectSettings; }

	private:
		ProjectProperties m_ProjectProperties;
		ProjectSettings m_ProjectSettings;
	};
}