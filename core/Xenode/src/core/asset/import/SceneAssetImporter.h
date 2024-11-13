#pragma once

#include <Core.h>
#include <core/asset/Asset.h>

namespace Xen
{
	class XEN_API SceneAssetImporter
	{
	public:
		static Ref<Asset> ImportSceneAsset(AssetMetadata* metadata);
	};
}

