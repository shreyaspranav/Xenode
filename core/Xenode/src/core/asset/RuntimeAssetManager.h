#pragma once
#include <Core.h>
#include "AssetManager.h"

namespace Xen {

	class RuntimeAssetManager : public AssetManager
	{
	public:
		RuntimeAssetManager();
		virtual ~RuntimeAssetManager();

		virtual Ref<Asset> GetAsset(AssetHandle handle) const override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		// Imports all assets from an "asset pack"
		bool ImportAssetsFromPack(const std::filesystem::path& filePath) override;
	private:
		AssetPtrRegistry m_PtrRegistry;
	};
}