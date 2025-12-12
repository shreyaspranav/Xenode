#pragma once

#include <Core.h>
#include "Asset.h"
#include "AssetMetadata.h"

namespace Xen
{
	// Asset Pointer Registry Type. Common to both runtime and in the editor
	using AssetPtrRegistry = UnorderedMap<AssetHandle, Ref<Asset>>;

	// The base asset manager. 
	// Depending if the game is in runtime or in the editor, separate implementations of the AssetManager in used.
	// 
	// "Importing" an asset means to load the asset from disk.
	// "Loading" an asset means to load the loaded asset data to the target(e.g. the GPU)
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