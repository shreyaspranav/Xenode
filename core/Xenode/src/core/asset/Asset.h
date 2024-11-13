#pragma once

#include <Core.h>
#include <core/app/Log.h>
#include <core/app/UUID.h>

namespace Xen
{
	using AssetHandle = UUID;

	enum class AssetType : uint8_t
	{
		None = 0,

		Scene,
		Texture2D,
		Shader,

		// TODO: I'll implement the asset type for scripts later.
		Script
	};

	class AssetUtil
	{
	public:
		static std::string ToAssetTypeString(AssetType type)
		{
			switch (type)
			{
			case Xen::AssetType::None:      return "None";
			case Xen::AssetType::Scene:     return "Scene";
			case Xen::AssetType::Texture2D: return "Texture2D";
			case Xen::AssetType::Shader:    return "Shader";
			case Xen::AssetType::Script:    return "Script";
			}

			XEN_ENGINE_LOG_ERROR("Unknown AssetType!");
			TRIGGER_BREAKPOINT;

			return "";
		}

		static AssetType ToAssetTypeFromString(const std::string& assetTypeString)
		{
			if (assetTypeString == "None")
				return AssetType::None;
			else if (assetTypeString == "Scene")
				return AssetType::Scene;
			else if (assetTypeString == "Texture2D")
				return AssetType::Texture2D;
			else if (assetTypeString == "Shader")
				return AssetType::Shader;
			else if (assetTypeString == "Script")
				return AssetType::Script;
			else
			{
				XEN_ENGINE_LOG_ERROR("Unknown assetTypeString!");
				TRIGGER_BREAKPOINT;

				return AssetType::None;
			}
		}
	};

	struct AssetMetadata
	{
		AssetType type;
		Size size;
		Vector<AssetHandle> dependencies;

		// These things shouldn't be in runtime:
		
		// Relative path from the assets directory.
		std::filesystem::path relPath;
		Buffer userData;
	};

	// Base class for all asset types.
	class XEN_API Asset
	{
	public:
		virtual AssetType GetAssetType() const = 0;

		AssetHandle GetAssetHandle() const { return handle; }
	protected:
		AssetHandle handle;
	};

	// Macros to define the implement the above methods:
#define DEFINE_ASSET_TYPE(type) \
	static AssetType GetStaticAssetType() { return type; } \
	virtual AssetType GetAssetType() const override { return GetStaticAssetType(); }
}