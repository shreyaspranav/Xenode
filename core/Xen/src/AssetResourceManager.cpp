#include "AssetResourceManager.h"

#include <project/ProjectManager.h>
#include <core/asset/AssetManagerUtil.h>
#include <core/asset/AssetUserData.h>

#include "ThumbnailGenerator.h"

struct AssetResourceManagerData
{
	std::filesystem::path assetPath;
	Xen::Ref<Xen::EditorAssetManager> assetManager;

	// std::thread t;

} assetResourceManagerState;

// Implementation: ------------------------------------------------------------------------------------
void AssetResourceManager::Init()
{
	Xen::Ref<Xen::Project> currentProject = Xen::ProjectManager::GetCurrentProject();
	std::filesystem::path currentProjectPath = Xen::ProjectManager::GetCurrentProjectPath();

	assetResourceManagerState.assetPath = currentProjectPath / currentProject->GetProjectSettings().relAssetDirectory;

	// This class is supposed to be used only in the editor.(not on the runtime)
	assetResourceManagerState.assetManager = Xen::AssetManagerUtil::GetEditorAssetManager();
}

void AssetResourceManager::Load()
{
	// This function needs to be run in a different thread.

	// assetResourceManagerState.t = std::thread(&AssetResourceManager::LoadDirectory, std::filesystem::directory_entry(assetResourceManagerState.assetPath));
	// assetResourceManagerState.t.join();

	AssetResourceManager::LoadDirectory(std::filesystem::directory_entry(assetResourceManagerState.assetPath));
	AssetResourceManager::GenerateThumbnails();

	assetResourceManagerState.assetManager->SerializeRegistry();
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
			std::filesystem::path relativePath = std::filesystem::relative(directoryEntry.path(), assetResourceManagerState.assetPath);
			
			if(!assetResourceManagerState.assetManager->IsFileLoadedAsAsset(relativePath))
				bool loaded = assetResourceManagerState.assetManager->ImportAssetFromFile(relativePath);

			// TODO: Dispatch a AssetLoad Event or something.
		}
	}
}

void AssetResourceManager::GenerateThumbnails()
{
	// TODO: Determine the size of the thumbnails. 
	for (auto&& assetMetadataEntry : assetResourceManagerState.assetManager->GetAssetMetadataRegistry())
	{
		Xen::AssetHandle handle = assetMetadataEntry.first;
		Xen::AssetMetadata& metadata = assetMetadataEntry.second;

		if (metadata.type == Xen::AssetType::Texture2D)
		{
			Xen::TextureAssetUserData* textureAssetUserData = (Xen::TextureAssetUserData*)metadata.userData.buffer;
			Xen::Ref<Xen::Texture2D> textureAsset = Xen::AssetManagerUtil::GetAsset<Xen::Texture2D>(handle);
			textureAssetUserData->thumbnail = ThumbnailGenerator::GenerateTextureThumbnail(textureAsset, 120); // 8x Downsample the original texture.
		}
		else if (metadata.type == Xen::AssetType::Scene)
		{
			Xen::SceneAssetUserData* sceneAssetUserData = (Xen::SceneAssetUserData*)metadata.userData.buffer;
			Xen::Ref<Xen::Scene> sceneAsset = Xen::AssetManagerUtil::GetAsset<Xen::Scene>(handle);
			sceneAssetUserData->thumbnail = ThumbnailGenerator::GenerateSceneThumbnail(sceneAsset, sceneAssetUserData->editorCameraTransform, 120, 120); // 120x120 thumbnail
		}
	}
}
