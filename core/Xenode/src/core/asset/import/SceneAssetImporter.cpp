#include "pch"
#include "SceneAssetImporter.h"

#include <project/ProjectManager.h>
#include <core/scene/SceneSerializer.h>

namespace Xen
{
	Vector<std::byte> SceneAssetImporter::ImportSceneAsset(AssetMetadata* metadata)
	{
		// This shouldn't happen in runtime!
		EditorAssetMetadata* editorAssetMetadata = (EditorAssetMetadata*)metadata;

		Ref<Project> currentProject = ProjectManager::GetCurrentProject();
		std::filesystem::path assetPath = currentProject->GetProjectSettings().relAssetDirectory;

		std::filesystem::path completePath = ProjectManager::GetCurrentProjectPath() / assetPath / editorAssetMetadata->relPath;

		// Calculate the size of the scene:
		std::ifstream inputStream(completePath);
		inputStream.seekg(0, std::ios::end);
		Size s = inputStream.tellg();
		inputStream.seekg(0, std::ios::beg);

		Vector<std::byte> buffer(s);
		inputStream.read(reinterpret_cast<char*>(buffer.data()), s);
		inputStream.close();

		Component::Transform editorCameraTransform = SceneSerializer::GetEditorCameraTransform(std::string(reinterpret_cast<char*>(buffer.data())));

		editorAssetMetadata->editorSpecific = EditorSceneMetadata(editorCameraTransform);
		editorAssetMetadata->size = s;

		return buffer;
	}
	Ref<Asset> SceneAssetImporter::LoadSceneAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata)
	{
		Ref<Scene> sceneAsset = std::make_shared<Scene>();
		SceneSerializer::DeserializeYAML(sceneAsset, std::string(reinterpret_cast<const char*>(buffer.data())));
		return sceneAsset;
	}
}