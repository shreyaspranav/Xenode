#pragma once
#include <Core.h>
#include <core/asset/Asset.h>

namespace Xen
{
	class XEN_API TextureAssetImporter
	{
	public:
		static Ref<Asset> ImportTextureAsset(const AssetMetadata& metadata);
	private:
		static Ref<Asset> ImportTexture2D(const AssetMetadata& metadata, const std::filesystem::path& completeFilePath);
	};
}