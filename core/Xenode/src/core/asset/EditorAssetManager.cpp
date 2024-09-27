#include "pch"
#include "EditorAssetManager.h"

#include <project/ProjectManager.h>

#include "AssetImporter.h"

namespace Xen
{
	// TODO: Find all the common extensions and fill this.
	std::unordered_map<std::string, AssetType> fileExtensions =
	{
		// Texture2D file extensions:
		{ ".png",     AssetType::Texture2D },
		{ ".jpg",     AssetType::Texture2D },
		{ ".jpeg",    AssetType::Texture2D },

		// Scene file extension:
		{ ".xen",     AssetType::Scene },

		// Normal Shader file extension:
		{ ".shader",  AssetType::Shader },
	};

	EditorAssetManager::EditorAssetManager()
	{

	}
	EditorAssetManager::~EditorAssetManager()
	{

	}
	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle) const
	{
		auto&& it = m_PtrRegistry.find(handle);
		Ref<Asset> assetToReturn = (*it).second;
		
		return assetToReturn;
	}
	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return m_PtrRegistry.find(handle) != m_PtrRegistry.end();
	}
	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_PtrRegistryLoaded.find(handle) != m_PtrRegistryLoaded.end();
	}
	bool EditorAssetManager::ImportAssetsFromPack(const std::filesystem::path& filePath)
	{
		// TODO: Will implement asset packs later.
		return false;
	}
	bool EditorAssetManager::ImportAssetsFromFiles(const std::filesystem::path& filePath)
	{
		std::filesystem::directory_entry directory(filePath);
		
		if (!directory.exists())
			return false;

		if (directory.is_directory())
		{
			// Import all the files in the directory as assets.
		}
		else
		{
			std::string fileExtension = directory.path().extension().string();
			
			// Return false if the file has a unknown file extension
			if (fileExtensions.find(fileExtension) == fileExtensions.end())
				return false;

			AssetMetadata metadata;
			// metadata.
		}

		return false;
	}
}
