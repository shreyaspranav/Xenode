#include "AssetResourceManager.h"

#include <project/ProjectManager.h>
#include <core/asset/AssetManagerUtil.h>

#include "ThumbnailGenerator.h"

struct AssetResourceManagerData
{
	std::filesystem::path assetPath;
	Xen::Ref<Xen::EditorAssetManager> assetManager;

	std::thread assetLoadThread;

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

	AssetResourceManager::LoadDirectory(std::filesystem::directory_entry(assetResourceManagerState.assetPath));
	AssetResourceManager::GenerateThumbnails();

	assetResourceManagerState.assetManager->SerializeRegistry();


	//assetResourceManagerState.assetLoadThread =
	//	std::thread(
	//		[]()
	//		{
	//			AssetResourceManager::LoadDirectory(std::filesystem::directory_entry(assetResourceManagerState.assetPath));
	//			AssetResourceManager::GenerateThumbnails();

	//			assetResourceManagerState.assetManager->SerializeRegistry();
	//		}
	//	);
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
	for (auto&& [handle, metadata] : assetResourceManagerState.assetManager->GetAssetMetadataRegistry())
	{
		if (metadata.type == Xen::AssetType::Texture2D)
		{
			Xen::Ref<Xen::Texture2D> textureAsset = Xen::AssetManagerUtil::GetAsset<Xen::Texture2D>(handle);
			metadata.thumbnail = ThumbnailGenerator::GenerateTextureThumbnail(textureAsset, 120); // 8x Downsample the original texture.
		}
		else if (metadata.type == Xen::AssetType::Scene)
		{
			Xen::EditorSceneMetadata* editorSceneMetadata = std::get_if<Xen::EditorSceneMetadata>(&metadata.editorSpecific);

			Xen::Ref<Xen::Scene> sceneAsset = Xen::AssetManagerUtil::GetAsset<Xen::Scene>(handle);
			metadata.thumbnail = ThumbnailGenerator::GenerateSceneThumbnail(sceneAsset, editorSceneMetadata->editorCameraTransform, 120, 120); // 120x120 thumbnail
		}
	}
}
