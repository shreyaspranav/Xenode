#pragma once
#include <Core.h>

#include "Project.h"
#include <filesystem>
#include <core/asset/AssetManager.h>

namespace Xen 
{
	class XEN_API ProjectManager
	{
	public:
		// Creates a project on disk with the given properties.
		static Ref<Project> CreateProjectOnDisk(const std::filesystem::path& rootPath, const ProjectProperties& properties);

		// Loads the existing project and returns its instance. (Also initializes the asset manager instance)
		static Ref<Project> LoadProject(const std::filesystem::path pathToProjectFile);

		// Returns the instance of the current project
		static const Ref<Project>& GetCurrentProject();

		// Returns the instance of the current asset manager
		static const Ref<AssetManager>& GetCurrentAssetManager();

		// Returns the path of the current project
		static const std::filesystem::path& GetCurrentProjectPath();

		// Unloads the current project
		static void UnloadProject();
	};

}