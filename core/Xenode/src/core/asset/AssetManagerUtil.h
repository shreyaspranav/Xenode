#pragma once
#include <Core.h>

#include <project/ProjectManager.h>

#include "AssetManager.h"
#include "EditorAssetManager.h"
#include "RuntimeAssetManager.h"

namespace Xen
{
	class XEN_API AssetManagerUtil
	{
	public:
		static Ref<EditorAssetManager> GetEditorAssetManager()
		{
			Ref<AssetManager> currentAssetManager = ProjectManager::GetCurrentAssetManager();
			return std::dynamic_pointer_cast<EditorAssetManager>(currentAssetManager);
		}

		static Ref<RuntimeAssetManager> GetRuntimeAssetManager()
		{
			Ref<AssetManager> currentAssetManager = ProjectManager::GetCurrentAssetManager();
			return std::dynamic_pointer_cast<RuntimeAssetManager>(currentAssetManager);
		}

		template<typename AssetType>
		static Ref<AssetType> GetSpecificAsset(const Ref<Asset>& asset)
		{
			return std::dynamic_pointer_cast<AssetType>(asset);
		}

		template<typename AssetType>
		static Ref<AssetType> GetAsset(AssetHandle handle)
		{
			Ref<AssetManager> currentAssetManager = ProjectManager::GetCurrentAssetManager();
			Ref<Asset> asset = currentAssetManager->GetAsset(handle);

			return GetSpecificAsset<AssetType>(asset);
		}

	};
}