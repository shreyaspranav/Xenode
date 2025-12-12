#pragma once
#include <Core.h>

#include "AssetMetadata.h"

namespace Xen
{
	class XEN_API AssetImporter
	{
	public:
		// Import asset from a file and returns a buffer of preprocessed data.
		static Vector<std::byte> ImportAsset(AssetMetadata* metadata);

		// Take a buffer of data and return the corresponding asset.
		static Ref<Asset> LoadAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata);
	};
}