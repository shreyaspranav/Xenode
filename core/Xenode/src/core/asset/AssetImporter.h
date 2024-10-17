#pragma once
#include <Core.h>

#include "Asset.h"

namespace Xen
{
	class XEN_API AssetImporter
	{
	public:
		// Import asset from a file and returns a buffer of preprocessed data.
		static Ref<Asset> ImportAsset(AssetMetadata* metadata);
	};
}