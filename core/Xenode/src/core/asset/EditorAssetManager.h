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

		// IMPORTANT: all the file paths are either relative to the current project's asset directory
		// or absolute.

		// Imports all assets from an "asset pack"
		bool ImportAssetsFromPack(const std::filesystem::path& filePath) override;
		bool ImportAssetFromFile(const std::filesystem::path& filePath);

		void SerializeRegistry();

		inline AssetPtrRegistry GetLoadedAssetRegistry() { return m_PtrRegistryLoaded; }

	private:
		AssetPtrRegistry m_PtrRegistry;
		AssetPtrRegistry m_PtrRegistryLoaded;  // "Loaded" refers to if the asset is ready to use.

		AssetMetadataRegistry m_MetadataRegistry;
	};

	class AssetRegistrySerializer
	{
	public:
		static void Serialize(const AssetMetadataRegistry& registry, const std::filesystem::path& filePath);
		static void Deserialize(AssetMetadataRegistry& registry, const std::filesystem::path& filePath);
	};
}
