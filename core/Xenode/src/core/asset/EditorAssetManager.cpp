#include "pch"
#include "EditorAssetManager.h"

#include <core/app/Timer.h>
#include <project/ProjectManager.h>

#include "AssetImporter.h"

#pragma warning(push)
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)

#include <yaml-cpp/yaml.h>

#pragma warning(pop)

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
		m_AssetFileTreeRoot = new AssetHandleFileTreeNode("assets");

		if (std::filesystem::exists(GetAssetRegistryFilePath()))
		{
			AssetRegistrySerializer::Deserialize(m_MetadataRegistry, m_FileRegistry, GetAssetRegistryFilePath());
			for (auto&& [handle, metadata] : m_MetadataRegistry)
				ImportAssetFromFileBase(handle, metadata);
		}

	}
	EditorAssetManager::~EditorAssetManager()
	{
		// TODO: Write a custom memory allocator.
		// Deallocate all the user data
		for (auto&& metadataEntry : m_MetadataRegistry)
			metadataEntry.second.userData.Free();
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
			if(handle) // Log only when non zero handles are requested
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
		if (IsFileLoadedAsAsset(filePath))
		{
			XEN_ENGINE_LOG_WARN("The file {0} is already loaded as an asset!", filePath.string());
			return true;
		}

		AssetMetadata metadata;

		std::string fileExtension = filePath.extension().string();

		metadata.relPath = filePath;
		metadata.type = fileExtensions[fileExtension];

		return ImportAssetFromFileBase(AssetHandle(), metadata); // Generate a new handle
	}
	bool EditorAssetManager::IsFileLoadedAsAsset(const std::filesystem::path& filePath)
	{
		return m_FileRegistry.find(filePath) != m_FileRegistry.end();
	}
	void EditorAssetManager::SerializeRegistry()
	{
		AssetRegistrySerializer::Serialize(m_MetadataRegistry, GetAssetRegistryFilePath());
	}

	bool EditorAssetManager::ImportAssetFromFileBase(AssetHandle handle, AssetMetadata& metadata)
	{
		Timer t;

		std::string fileExtension = metadata.relPath.extension().string();

		// Return false if the file has a unknown file extension
		if (fileExtensions.find(fileExtension) == fileExtensions.end())
			return false;

		Ref<Asset> asset = AssetImporter::ImportAsset(&metadata);

		t.Stop();
		XEN_ENGINE_LOG_INFO("Asset '{0}' Imported From disk at {1}ms:", metadata.relPath.string(), t.GetElapedTime() / 1000.0f);

		if (!asset)
			return false;
		else
		{
			// If the handle or the filepath already exists, the following code does nothing.
			m_PtrRegistry.insert({ handle, asset });
			m_PtrRegistryLoaded.insert({ handle, asset }); // The asset is already loaded.
			m_FileRegistry.insert({ metadata.relPath, handle });

			// Add the asset to the Asset File Tree.
			AddAssetToFileTree(handle, metadata.relPath);

			m_MetadataRegistry.insert({ handle, metadata });
			return true;
		}
	}

	// Private Methods: ----------------------------------------------------------------------------------------------------------
	void EditorAssetManager::AddAssetToFileTree(AssetHandle handle, const std::filesystem::path& path)
	{
		// Do nothing if the file exists in the tree.
		// if (m_FileRegistry.find(path) != m_FileRegistry.end())
		// 	return;

		std::string parentPathString = path.parent_path().string();
		
		// Replace \ by / so that all paths are uniform across platforms
		std::replace(parentPathString.begin(), parentPathString.end(), '\\', '/');

		AssetHandleFileTreeNode* currentNode = m_AssetFileTreeRoot;

		std::stringstream pathStream(parentPathString);
		std::string currentFolder;
		Vector<std::string> folders;

		// TODO: Make sure to somehow support '/' too.
		while (std::getline(pathStream, currentFolder, '/'))
		{
			AssetHandleFileTreeNode* currentFolderNode = GetFolderPresentInChildren(currentNode, currentFolder);

			// There is no child node named 'currentFolder', create a new node:
			if (!currentFolderNode)
			{
				AssetHandleFileTreeNode* folderNode = new AssetHandleFileTreeNode(currentFolder);
				
				currentNode->children.push_back(folderNode);
				folderNode->parent = currentNode;

				currentNode = folderNode;
			}
			// There is a folder named 'currentFolder', use that.
			else
				currentNode = currentFolderNode;
		}

		AssetHandleFileTreeNode* assetNode = new AssetHandleFileTreeNode(handle);
		currentNode->children.push_back(assetNode);
	}

	AssetHandleFileTreeNode* EditorAssetManager::GetFolderPresentInChildren(AssetHandleFileTreeNode* parentNode, const std::string& folderName)
	{
		for (AssetHandleFileTreeNode* childNode : parentNode->children)
			if (childNode->type == AssetHandleFileTreeNodeType::Folder && childNode->folderName == folderName)
				return childNode;

		return nullptr;
	}

	std::filesystem::path EditorAssetManager::GetAssetRegistryFilePath()
	{
		std::filesystem::path fileName = ProjectManager::GetCurrentProject()->GetProjectProperties().name + ".areg";
		std::filesystem::path filePath = ProjectManager::GetCurrentProjectPath();

		return filePath / fileName;
	}

	// ---------------------------------------------------------------------------------------------------------------------------
	// AssetRegistrySerializer Implementation: -----------------------------------------------------------------------------------

	// operator<< for AssetMetadata
	YAML::Emitter& operator<<(YAML::Emitter& emitter, const AssetMetadata& metadata)
	{
		emitter << YAML::BeginMap;

		emitter << YAML::Key << "AssetType" << YAML::Value << AssetUtil::ToAssetTypeString(metadata.type);
		emitter << YAML::Key << "Size" << YAML::Value << metadata.size;

		std::string relPath = metadata.relPath.string();
#ifdef XEN_PLATFORM_WINDOWS
		std::replace(relPath.begin(), relPath.end(), '\\', '/');
#endif
		emitter << YAML::Key << "RelativePath" << YAML::Value << relPath;
		
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

		std::string relPath = metadataNode["RelativePath"].as<std::string>();
#ifdef XEN_PLATFORM_WINDOWS
		std::replace(relPath.begin(), relPath.end(), '/', '\\');
#endif
		metadata.relPath = relPath;

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
	void AssetRegistrySerializer::Deserialize(AssetMetadataRegistry& registry, AssetFileRegistry& fileRegistry, const std::filesystem::path& filePath)
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
