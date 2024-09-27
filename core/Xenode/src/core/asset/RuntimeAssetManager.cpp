#include "pch"
#include "RuntimeAssetManager.h"

namespace Xen
{
	RuntimeAssetManager::RuntimeAssetManager()
	{
	}
	RuntimeAssetManager::~RuntimeAssetManager()
	{
	}
	Ref<Asset> RuntimeAssetManager::GetAsset(AssetHandle handle) const
	{
		return Ref<Asset>();
	}
	bool RuntimeAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return false;
	}
	bool RuntimeAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return false;
	}
	bool RuntimeAssetManager::ImportAssetsFromPack(const std::filesystem::path& filePath)
	{
		return false;
	}
}