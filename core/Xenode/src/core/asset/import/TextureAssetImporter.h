#pragma once
#include <Core.h>
#include <core/asset/AssetMetadata.h>

namespace Xen
{
	class XEN_API TextureAssetImporter
	{
	public:
		static Vector<std::byte> ImportTextureAsset(AssetMetadata* metadata);
		static Ref<Asset> LoadTextureAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata);
	private:
		static Vector<std::byte> ImportTexture2D(AssetMetadata* metadata, const std::filesystem::path& completeFilePath);
	};
}