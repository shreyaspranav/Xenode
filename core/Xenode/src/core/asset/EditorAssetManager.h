#pragma once
#include <Core.h>

#include "AssetManager.h"

namespace Xen
{
	class EditorAssetManager : public AssetManager
	{
	public:
		EditorAssetManager();
		virtual ~EditorAssetManager();

		virtual Ref<Asset> GetAsset(AssetHandle handle) const override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;
		
		// Imports all assets from an "asset pack"
		bool ImportAssetsFromPack(const std::filesystem::path& filePath) override;

		// If a file, imports the file as an asset, If a directory, imports all the files in
		// the directory as an assets.
		bool ImportAssetsFromFiles(const std::filesystem::path& filePath);

	private:
		AssetPtrRegistry m_PtrRegistry;
		AssetPtrRegistry m_PtrRegistryLoaded;  // "Loaded" refers to if the asset is ready to use.

		AssetMetadataRegistry m_MetadataRegistry;
	};
}
