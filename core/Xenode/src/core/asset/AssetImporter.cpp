#include "pch"
#include "AssetImporter.h"

#include "import/TextureAssetImporter.h"
#include "import/ShaderAssetImporter.h"
#include "import/SceneAssetImporter.h"

#include <core/app/Log.h>

namespace Xen
{
	// AssetImportFn is a function that takes in a AssetMetadata* and returns Vector<std::byte>
	using AssetImportFn = std::function<Vector<std::byte>(AssetMetadata*)>;

	// AssetLoadFn is a function that takes in a AssetMetadata* and returns Vector<std::byte>
	using AssetLoadFn = std::function<Ref<Asset>(const Vector<std::byte>&, AssetMetadata*)>;

	UnorderedMap<AssetType, AssetImportFn> importFns = 
	{
		{ AssetType::Texture2D, TextureAssetImporter::ImportTextureAsset },
		{ AssetType::Shader,    ShaderAssetImporter::ImportShaderAsset },
		{ AssetType::Scene,     SceneAssetImporter::ImportSceneAsset }
	};

	UnorderedMap<AssetType, AssetLoadFn> loadFns =
	{
		{ AssetType::Texture2D, TextureAssetImporter::LoadTextureAsset },
		{ AssetType::Shader,    ShaderAssetImporter::LoadShaderAsset },
		{ AssetType::Scene,     SceneAssetImporter::LoadSceneAsset }
	};

	Vector<std::byte> AssetImporter::ImportAsset(AssetMetadata* metadata)
	{
		if(importFns.find(metadata->type) != importFns.end())
			return importFns[metadata->type](metadata);
		else
		{
			XEN_ENGINE_LOG_ERROR("Asset Importer Function for {0} not found!, File: {1}", AssetUtil::ToAssetTypeString(metadata->type), ((EditorAssetMetadata*)metadata)->relPath.string());
			return Vector<std::byte>();
		}
	}
	Ref<Asset> AssetImporter::LoadAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata)
	{
		if (loadFns.find(metadata->type) != loadFns.end())
			return loadFns[metadata->type](buffer, metadata);
		else
		{
			XEN_ENGINE_LOG_ERROR("Asset Loader Function for {0} not found!, File: {1}", AssetUtil::ToAssetTypeString(metadata->type), ((EditorAssetMetadata*)metadata)->relPath.string());
			return nullptr;
		}
	}
}