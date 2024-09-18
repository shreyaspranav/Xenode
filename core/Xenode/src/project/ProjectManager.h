#pragma once
#include <Core.h>

#include "Project.h"
#include <filesystem>

namespace Xen 
{
	class XEN_API ProjectManager
	{
	public:
		// Creates a project on disk with the given properties.
		static Ref<Project> CreateProject(const std::filesystem::path& rootPath, const ProjectProperties& properties);

		// Loads the existing project and returns its instance.
		static Ref<Project> LoadProject(const std::filesystem::path pathToProjectFile);

		// Returns the instance of the current project
		static const Ref<Project>& GetCurrentProject();

		// Returns the path of the current project
		static const std::filesystem::path& GetCurrentProjectPath();

		// Unloads the current project
		static void UnloadProject();
	};

}