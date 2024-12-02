#include "pch"
#include "SceneAssetImporter.h"

#include <project/ProjectManager.h>
#include <core/scene/SceneSerializer.h>
#include <core/asset/AssetUserData.h>

namespace Xen
{
	Ref<Asset> SceneAssetImporter::ImportSceneAsset(AssetMetadata* metadata)
	{
		Ref<Project> currentProject = ProjectManager::GetCurrentProject();
		std::filesystem::path assetPath = currentProject->GetProjectSettings().relAssetDirectory;

		std::filesystem::path completePath = ProjectManager::GetCurrentProjectPath() / assetPath / metadata->relPath;

		// TODO: Implement serialization of scene type to the scene
		Ref<Scene> sceneAsset = std::make_shared<Scene>();
		Component::Transform editorCameraTransform = SceneSerializer::Deserialize(sceneAsset, completePath.string());

		// Calculate the size of the scene:
		std::ifstream inputStream(completePath);
		inputStream.seekg(0, std::ios::end);
		Size s = inputStream.tellg();
		inputStream.close();

		SceneAssetUserData* sceneUserData = new SceneAssetUserData();
		sceneUserData->editorCameraTransform = editorCameraTransform;

		metadata->size = s;

		metadata->userData.buffer = sceneUserData;
		metadata->userData.alloc = true;
		metadata->userData.size = sizeof(SceneAssetUserData);

		return sceneAsset;
	}
}