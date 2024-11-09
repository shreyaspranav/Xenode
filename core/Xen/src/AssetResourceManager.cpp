#include "AssetResourceManager.h"

#include <project/ProjectManager.h>
#include <core/asset/AssetManagerUtil.h>

struct AssetResourceManagerData
{
	std::filesystem::path assetPath;
	Xen::Ref<Xen::EditorAssetManager> assetManager;

} assetDiscoveryManagerState;

// Implementation: ------------------------------------------------------------------------------------
void AssetResourceManager::Init()
{
	Xen::Ref<Xen::Project> currentProject = Xen::ProjectManager::GetCurrentProject();
	std::filesystem::path currentProjectPath = Xen::ProjectManager::GetCurrentProjectPath();

	assetDiscoveryManagerState.assetPath = currentProjectPath / currentProject->GetProjectSettings().relAssetDirectory;

	// This class is supposed to be used only in the editor.(not on the runtime)
	assetDiscoveryManagerState.assetManager = Xen::AssetManagerUtil::GetEditorAssetManager();
}

void AssetResourceManager::Load()
{
	// This function needs to be run in a different thread.
	AssetResourceManager::LoadDirectory(std::filesystem::directory_entry(assetDiscoveryManagerState.assetPath));
}

void AssetResourceManager::StartFileWatcher()
{

}

// Private functions: ----------------------------------------------------------------------------------

// This function will run in a different thread.
void AssetResourceManager::LoadDirectory(const std::filesystem::directory_entry& directory)
{
	std::filesystem::recursive_directory_iterator iterator(directory);

	for (auto& directoryEntry : iterator)
	{
		if (directoryEntry.is_regular_file())
		{
			// Calculate the relative path from the project's asset directory.
			std::filesystem::path relativePath = std::filesystem::relative(directoryEntry.path(), assetDiscoveryManagerState.assetPath);
			
			bool loaded = assetDiscoveryManagerState.assetManager->ImportAssetFromFile(relativePath);

			// TODO: Dispatch a AssetLoad Event or something.
		}
	}
}
