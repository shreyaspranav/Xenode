#pragma once

#include <Core.h>
#include "Asset.h"

namespace Xen
{
	// In Production, maybe use unordered map for better average case performance. (And we don't care about the order anyway)
#ifdef XEN_PRODUCTION
	using AssetPtrRegistry       = UnorderedMap<AssetHandle, Ref<Asset>>;
	using AssetMetadataRegistry  = UnorderedMap<AssetHandle, AssetMetadata>;
#else
	using AssetPtrRegistry       = Map<AssetHandle, Ref<Asset>>;
	using AssetMetadataRegistry  = Map<AssetHandle, AssetMetadata>;
#endif
	// The base asset manager. 
	// Depending if the game is in runtime or in the editor, separate implementations of the AssetManager in used.
	//
	// If the game is in editor, the game can 
	//	-> Load assets from disk
	//	-> Reload assets from disk if the file is changed externally
	//	-> Keep track of file paths and handles
	// 
	// If the game is in the runtime, the game can
	//	-> Load assets only from so called "asset packs".
	// 
	// 
	class XEN_API AssetManager
	{
	public:
		// Returns the asset by asset handle. 
		virtual Ref<Asset> GetAsset(AssetHandle handle) const = 0;

		// An asset needs to be imported to be "valid" and the "valid" asset needs to be "loaded"
		// to be readily used in the game.
		virtual bool IsAssetHandleValid(AssetHandle handle) const = 0;
		virtual bool IsAssetLoaded(AssetHandle handle) const = 0;

		// Load assets from a asset pack should be common to all types of AssetManagers
		// Here we are loading assets because the data is optimized, meaning it requires no preprocessing.
		virtual bool ImportAssetsFromPack(const std::filesystem::path& path) = 0;
	};
}