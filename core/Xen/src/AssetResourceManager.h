#pragma once
#include <Xenode.h>

// TODO: Include this in Xenode.h
#include <core/asset/EditorAssetManager.h>

// This class discovers assets in the assets directory, loads them, builds cache and loads from cache
// If needed.
// The Assets are imported through AssetManager(EditorAssetManager to be precise), and 
// reimports(by some notification) them if the assets are changed.
class AssetResourceManager
{
public:
	// Sets the current asset path and asset manager instance.
	static void Init();

	// Loads all the assets in the assetPath to the asset manager.
	// This will load from cache if needed.
	// This will happen in an different thread.
	static void Load();

	// Starts the file watcher thread. When any changes are done to the 
	// asset file, an event is dispatched that the asset has changed.
	static void StartFileWatcher();
private:
	static void LoadDirectory(const std::filesystem::directory_entry& directory);
	static void GenerateThumbnails();
};