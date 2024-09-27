#pragma once
#include <Core.h>

#include "Asset.h"

namespace Xen
{
	class XEN_API AssetImporter
	{
	public:
		static Ref<Asset> ImportAsset(const AssetMetadata& metadata);
	};
}