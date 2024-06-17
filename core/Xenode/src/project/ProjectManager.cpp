#include "pch"
#include "ProjectManager.h"

#include "ProjectSerializer.h"
#include <core/scene/Scene.h>
#include <core/scene/SceneSerializer.h>

namespace Xen 
{
	// Struct to hold the state of the Project Manager.
	struct ProjectManagerData
	{
		Ref<Project> currentProject = nullptr;
		std::filesystem::path currentProjectPath;

	}projectManagerState;

	Ref<Project> ProjectManager::CreateProject(const std::filesystem::path& rootPath, const ProjectProperties& properties)
	{
		std::filesystem::path projectPath = rootPath / properties.name;

		if (!std::filesystem::exists(projectPath))
		{
			// Create a project instance:
			Ref<Project> project = Project::CreateProject(properties);
			projectManagerState.currentProject = project;

			// Serialize the project file:
			std::filesystem::create_directories(projectPath);
			ProjectSerializer::Serialize(project, projectPath);

			// Create the necessary subdirectories:
			ProjectSettings settings = project->GetProjectSettings();
			
			std::filesystem::create_directories(projectPath / settings.relAssetDirectory);
			std::filesystem::create_directories(projectPath / settings.relBuildDirectory);
			std::filesystem::create_directories(projectPath / settings.relCacheDirectory);

			// Serialize the Start Scene:
			SceneSerializer::Serialize(settings.startScene, (projectPath / settings.relStartScenePath).string());

			return project;
		}
		
		else 
		{
			XEN_ENGINE_LOG_ERROR("Project Directory Already Exists! ");
			return nullptr;
		}
	}
	Ref<Project> ProjectManager::LoadProject(const std::filesystem::path pathToProjectFile)
	{
		if (!std::filesystem::exists(pathToProjectFile))
		{
			XEN_ENGINE_LOG_ERROR("Project Directory Doesn't exist! ");
			return nullptr;
		}

		else if (!ProjectSerializer::IsValidProjectFile(pathToProjectFile))
		{
			XEN_ENGINE_LOG_ERROR("Not a valid Project File! ");
			return nullptr;
		}

		projectManagerState.currentProjectPath = pathToProjectFile.parent_path();

		ProjectProperties p;
		Ref<Project> project = Project::CreateProject(p);

		ProjectSerializer::Deserialize(project, pathToProjectFile);

		std::filesystem::path startScenePath = pathToProjectFile.parent_path() / project->GetProjectSettings().relStartScenePath;
		SceneSerializer::Deserialize(project->GetProjectSettings().startScene, startScenePath.string());

		projectManagerState.currentProject = project;

		return project;
	}
	const Ref<Project>& ProjectManager::GetCurrentProject()
	{
		return projectManagerState.currentProject;
	}
	const std::filesystem::path& ProjectManager::GetCurrentProjectPath()
	{
		return projectManagerState.currentProjectPath;
	}
	void ProjectManager::UnloadProject()
	{

	}
}