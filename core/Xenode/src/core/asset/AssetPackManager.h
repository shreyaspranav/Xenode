#pragma once
#include <Core.h>

#include "Asset.h"
#include "AssetManager.h"

namespace Xen
{
	// There are two types of asset packs, a "Runtime" asset pack and A "Editor" pack
	//
	// Runtime Asset pack:
	//	-> This type of pack contains one "metadata" file (<project_name>.xam) and 1 file that constains the binary data of all assets
	//	   In the future, this could be divided into many files if the size of that file is too huge.
	//	-> All the required assets are divided into 'n' "chunks" of files. The division is based on 
	//	   file size and no single asset needs to be divided into 2 or more files.
	//	-> The "chunks" just contain tightly packed binary data.
	//	-> Since the assets are addressed by AssetHandle, There is an another "metadata" file
	//	   that contains: AssetHandle, The file index(the chunk), the offset and the size of the asset.
	// 
	// Editor Asset pack:
	//	-> Same as a runtime asset pack, but the "chunks" are more text based
	//	-> The "metadata" file should contain more information of a particular asset(eg. its type, and other data).

	struct AssetPackEntry
	{
		AssetType type;

		// fileIndex starts from 1. If 0, there is only one asset file.
		uint32_t fileIndex;
		Size offset, size;
	};

#ifdef XEN_PRODUCTION
	using AssetPackRegistry = UnorderedMap<AssetHandle, AssetPackEntry>;
#else
	using AssetPackRegistry = Map<AssetHandle, AssetPackEntry>;
#endif

	class XEN_API AssetPackManager
	{
	public:
		// Creates the asset pack in the current project's build directory
		// static void CreateAssetPack(const AssetMetadataRegistry& metadataRegistry);
	};
}