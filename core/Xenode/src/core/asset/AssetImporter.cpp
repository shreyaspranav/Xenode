#include "pch"
#include "AssetImporter.h"

#include "import/TextureAssetImporter.h"
#include "import/ShaderAssetImporter.h"
#include "import/SceneAssetImporter.h"

#include <core/app/Log.h>

namespace Xen
{
	// AssetImportFn is a function that takes in a const reference of std::filesystem::path and returns Ref<Asset>
	using AssetImportFn = std::function<Ref<Asset>(AssetMetadata*)>;

	// A map of all AssetTypes and AssetImportFn's to lookup the right asset import function.
	std::unordered_map<AssetType, AssetImportFn> importFns = 
	{
		{ AssetType::Texture2D, TextureAssetImporter::ImportTextureAsset },
		{ AssetType::Shader,    ShaderAssetImporter::ImportShaderAsset },
		{ AssetType::Scene,     SceneAssetImporter::ImportSceneAsset }
	};

	Ref<Asset> AssetImporter::ImportAsset(AssetMetadata* metadata)
	{
		if(importFns.find(metadata->type) != importFns.end())
			return importFns[metadata->type](metadata);
		else
		{
			XEN_ENGINE_LOG_ERROR("Asset Importer Function for {0} not found!", AssetUtil::ToAssetTypeString(metadata->type));
			return nullptr;
		}
	}
}