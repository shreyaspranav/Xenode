#pragma once

#include <Core.h>
#include <core/asset/AssetMetadata.h>

namespace Xen
{
	class XEN_API SceneAssetImporter
	{
	public:
		static Vector<std::byte> ImportSceneAsset(AssetMetadata* metadata);
		static Ref<Asset> LoadSceneAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata);
	};
}

