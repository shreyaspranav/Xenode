#include "pch"
#include "ProjectSerializer.h"

// Ignore Warnings related to dll linking
#pragma warning(push)

#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#include <yaml-cpp/yaml.h>

#pragma warning(pop)

namespace Xen
{
	void ProjectSerializer::Serialize(const Ref<Project>& project, const std::filesystem::path& projectPath)
	{
		ProjectProperties projectProperties = project->GetProjectProperties();
		ProjectSettings projectSettings = project->GetProjectSettings();

		YAML::Emitter yaml;
		{
			yaml << YAML::BeginMap;
			yaml << YAML::Key << "Project" << YAML::Value;

			{
				yaml << YAML::BeginMap;

				// Fields from ProjectProperties struct:
				yaml << YAML::Key << "Name" << YAML::Value << projectProperties.name;
				yaml << YAML::Key << "GameType" << YAML::Value << ProjectUtil::ToGameTypeString(projectProperties.gameType);
				yaml << YAML::Key << "ScriptingLanguage" << YAML::Value << ProjectUtil::ToScriptLangString(projectProperties.scriptLang);

				// Fields from ProjectSettings struct:
				yaml << YAML::Key << "AssetsPath" << YAML::Value << projectSettings.relAssetDirectory.string();
				yaml << YAML::Key << "BuildPath" << YAML::Value << projectSettings.relBuildDirectory.string();
				yaml << YAML::Key << "CachePath" << YAML::Value << projectSettings.relCacheDirectory.string();
				yaml << YAML::Key << "StartScenePath" << YAML::Value << projectSettings.relStartScenePath.string();
				
				yaml << YAML::EndMap;
			}
			
			yaml << YAML::EndMap; // ROOT
		}
		
		std::string projectName = projectProperties.name;

		std::ofstream outputStream(projectPath / (projectName + ".xenproject"));
		outputStream << yaml.c_str();
		outputStream.close();
	}

	void ProjectSerializer::Deserialize(const Ref<Project>& project, const std::filesystem::path& projectFilePath)
	{
		std::ifstream inputFileStream(projectFilePath);
		std::stringstream sceneStringData;

		sceneStringData << inputFileStream.rdbuf();

		YAML::Node sceneData = YAML::Load(sceneStringData);

		if (sceneData["Project"])
		{
			YAML::Node rootNode = sceneData["Project"];

			ProjectProperties& projectProperties = project->GetProjectProperties();
			ProjectSettings& projectSettings = project->GetProjectSettings();

			// Deserialize ProjectProperties:
			projectProperties.name = rootNode["Name"].as<std::string>();
			projectProperties.gameType = ProjectUtil::ToGameTypeFromString(rootNode["GameType"].as<std::string>());
			projectProperties.scriptLang = ProjectUtil::ToScriptLangFromString(rootNode["ScriptingLanguage"].as<std::string>());

			// Deserialize ProjectSettings:
			projectSettings.relAssetDirectory = std::filesystem::path(rootNode["AssetsPath"].as<std::string>());
			projectSettings.relBuildDirectory = std::filesystem::path(rootNode["BuildPath"].as<std::string>());
			projectSettings.relCacheDirectory = std::filesystem::path(rootNode["CachePath"].as<std::string>());
			projectSettings.relStartScenePath = std::filesystem::path(rootNode["StartScenePath"].as<std::string>());
		}
		else
		{
			XEN_ENGINE_LOG_ERROR("Not a valid project!");
			TRIGGER_BREAKPOINT;
		}
	}

	bool ProjectSerializer::IsValidProjectFile(const std::filesystem::path& projectFilePath)
	{
		std::ifstream inputFileStream(projectFilePath);
		std::stringstream sceneStringData;

		sceneStringData << inputFileStream.rdbuf();

		YAML::Node sceneData = YAML::Load(sceneStringData);

		return sceneData["Project"] ? true : false;
	}
}