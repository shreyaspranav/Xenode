#include "pch"
#include "EditorAssetManager.h"

#include <core/app/Timer.h>
#include <project/ProjectManager.h>

#include "AssetImporter.h"

#include <yaml-cpp/yaml.h>

namespace Xen
{
	// Map of all the file extensions and their asset types
	// TODO: Find all the common extensions and fill this.
	UnorderedMap<std::string, AssetType> fileExtensions =
	{
		// Texture2D file extensions:
		{ ".png",     AssetType::Texture2D },
		{ ".jpg",     AssetType::Texture2D },
		{ ".jpeg",    AssetType::Texture2D },

		// Scene file extension:
		{ ".xen",     AssetType::Scene },

		// Normal Shader file extension:
		{ ".shader",  AssetType::Shader },
	};

	// EditorAssetManager Implementation: -------------------------------------------------------------------------
	EditorAssetManager::EditorAssetManager()
	{

	}
	EditorAssetManager::~EditorAssetManager()
	{

	}
	Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle) const
	{
		auto&& it = m_PtrRegistry.find(handle);
		if (it != m_PtrRegistry.end())
		{
			Ref<Asset> assetToReturn = (*it).second;
			return assetToReturn;
		}
		else
		{
			XEN_ENGINE_LOG_ERROR("Handle '{0}' is invalid!", handle);
			return nullptr;
		}
		
	}
	bool EditorAssetManager::IsAssetHandleValid(AssetHandle handle) const
	{
		return m_PtrRegistry.find(handle) != m_PtrRegistry.end();
	}
	bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
	{
		return m_PtrRegistryLoaded.find(handle) != m_PtrRegistryLoaded.end();
	}
	bool EditorAssetManager::ImportAssetsFromPack(const std::filesystem::path& filePath)
	{
		// TODO: Will implement asset packs later.
		return false;
	}

	bool EditorAssetManager::ImportAssetFromFile(const std::filesystem::path& filePath)
	{
		Timer t;

		std::string fileExtension = filePath.extension().string();
		
		// Return false if the file has a unknown file extension
		if (fileExtensions.find(fileExtension) == fileExtensions.end())
			return false;

		AssetMetadata metadata;

		metadata.relPath = filePath;
		metadata.type = fileExtensions[fileExtension];

		Ref<Asset> asset = AssetImporter::ImportAsset(&metadata);

		t.Stop();
		XEN_ENGINE_LOG_INFO("Asset '{0}' Imported From disk at {1}ms:", filePath.string(), t.GetElapedTime() / 1000.0f);

		if(!asset)
			return false;
		else
		{
			AssetHandle handle; // Generate a new handle
			m_PtrRegistry.insert({ handle, asset });
			m_PtrRegistryLoaded.insert({ handle, asset }); // The asset is already loaded.

			// Change the metadata's filepath relative to the assets directory

			m_MetadataRegistry.insert({ handle, metadata });
			return true;
		}
	}
	void EditorAssetManager::SerializeRegistry()
	{
		AssetRegistrySerializer::Serialize(m_MetadataRegistry, "test.asset_dir");
	}
	// ---------------------------------------------------------------------------------------------------------------------------
	// AssetRegistrySerializer Implementation: -----------------------------------------------------------------------------------

	// operator<< for AssetMetadata
	YAML::Emitter& operator<<(YAML::Emitter& emitter, const AssetMetadata& metadata)
	{
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "AssetType" << YAML::Value << AssetUtil::ToAssetTypeString(metadata.type);
		emitter << YAML::Key << "Size" << YAML::Value << metadata.size;
		emitter << YAML::Key << "RelativePath" << YAML::Value << metadata.relPath.string();
		
		emitter << YAML::Key << "Dependencies" << YAML::Value;
		{
			emitter << YAML::Flow << YAML ::BeginSeq;

			for (AssetHandle handle : metadata.dependencies)
				emitter << handle; // calls operator uint64_t()

			emitter << YAML::EndSeq;
		}

		emitter << YAML::EndMap;

		return emitter;
	}

	static void GetAssetMetadata(const YAML::Node& node, AssetMetadata& metadata)
	{
		auto&& metadataNode = node["Metadata"];

		std::string assetType = metadataNode["AssetType"].as<std::string>();

		metadata.type = AssetUtil::ToAssetTypeFromString(assetType);
		metadata.size = metadataNode["Size"].as<Size>();
		metadata.relPath = metadataNode["RelativePath"].as<std::string>();

		for (auto&& assetHandle : metadataNode["Dependencies"])
		{
			AssetHandle handle = assetHandle.as<uint64_t>();
			metadata.dependencies.push_back(handle);
		}
	}

	void AssetRegistrySerializer::Serialize(const AssetMetadataRegistry& registry, const std::filesystem::path& filePath)
	{
		YAML::Emitter yaml;
		
		yaml << YAML::BeginMap;
		yaml << YAML::Key << "AssetRegistry" << YAML::Value;
		{
			yaml << YAML::BeginSeq;
			for (auto& [assetHandle, metadata] : registry)
			{
				yaml << YAML::BeginMap;
				
				yaml << YAML::Key << "AssetHandle" << YAML::Value << assetHandle;
				yaml << YAML::Key << "Metadata" << YAML::Value << metadata;
				
				yaml << YAML::EndMap;
			}
			yaml << YAML::EndSeq;
		}
		yaml << YAML::EndMap; // ROOT;
		
		std::ofstream outputStream(filePath);
		outputStream << yaml.c_str();
		outputStream.close();
	}
	void AssetRegistrySerializer::Deserialize(AssetMetadataRegistry& registry, const std::filesystem::path& filePath)
	{
		std::ifstream inputStream(filePath);
		if (!inputStream)
		{
			XEN_ENGINE_LOG_ERROR("Unable to open file: {0}", filePath.string());
			TRIGGER_BREAKPOINT;
		}

		YAML::Node rootNode = YAML::Load(inputStream);
		if (rootNode["AssetRegistry"])
		{
			for (auto&& asset : rootNode["AssetRegistry"])
			{
				AssetHandle handle = asset["AssetHandle"].as<uint64_t>();
				AssetMetadata metadata;

				GetAssetMetadata(asset, metadata);
				registry.insert({ handle, metadata });
			}
		}
		else
		{
			XEN_ENGINE_LOG_ERROR("Not a valid Asset Registry: {0}", filePath.string());
			TRIGGER_BREAKPOINT;
		}
	}
}
