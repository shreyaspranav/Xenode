#pragma once
#include <Core.h>

#include "AssetManager.h"

namespace Xen
{
	enum class AssetHandleFileTreeNodeType { Folder, File };

	// A tree data structure is used to store the file structure of the assets in the assets directory.
	struct AssetHandleFileTreeNode
	{
		// Either the handle needs to be non zero and folderName and children needs to be empty(represents a leaf in the tree), 
		// or the handle needs to zero and folderName and children needs to non empty()

		AssetHandleFileTreeNodeType type;

		std::string folderName;

		Vector<AssetHandleFileTreeNode*> children;
		AssetHandleFileTreeNode* parent;

		AssetHandle handle = 0;

		AssetHandleFileTreeNode(const std::string& folderName = "")
			:folderName(folderName), type(AssetHandleFileTreeNodeType::Folder), parent(nullptr)
		{}

		AssetHandleFileTreeNode(AssetHandle handle)
			:handle(handle), type(AssetHandleFileTreeNodeType::File), parent(nullptr)
		{}

		~AssetHandleFileTreeNode()
		{
			for (AssetHandleFileTreeNode* node : children)
				delete node;
		}
	};

	class EditorAssetManager : public AssetManager
	{
	public:
		EditorAssetManager();
		virtual ~EditorAssetManager();

		virtual Ref<Asset> GetAsset(AssetHandle handle) const override;

		virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		virtual bool IsAssetLoaded(AssetHandle handle) const override;

		// IMPORTANT: all the file paths are either relative to the current project's asset directory

		// Imports all assets from an "asset pack"
		bool ImportAssetsFromPack(const std::filesystem::path& filePath) override;
		bool ImportAssetFromFile(const std::filesystem::path& filePath);

		void SerializeRegistry();

		inline AssetPtrRegistry GetLoadedAssetRegistry()          { return m_PtrRegistryLoaded; }
		inline AssetMetadataRegistry GetAssetMetadataRegistry()   { return m_MetadataRegistry;  }

		inline AssetHandleFileTreeNode* GetAssetHandleFileTree()  { return m_AssetFileTreeRoot; }

	private:
		void AddAssetToFileTree(AssetHandle handle, const std::filesystem::path& path);
		AssetHandleFileTreeNode* GetFolderPresentInChildren(AssetHandleFileTreeNode* parentNode, const std::string& folderName);

	private:
		AssetPtrRegistry m_PtrRegistry;
		AssetPtrRegistry m_PtrRegistryLoaded;         // "Loaded" refers to if the asset is ready to use.
		AssetMetadataRegistry m_MetadataRegistry;
		
		AssetHandleFileTreeNode* m_AssetFileTreeRoot; // Asset File Tree: this tree contains assets stored as a tree according to their file structure.


	};

	class AssetRegistrySerializer
	{
	public:
		static void Serialize(const AssetMetadataRegistry& registry, const std::filesystem::path& filePath);
		static void Deserialize(AssetMetadataRegistry& registry, const std::filesystem::path& filePath);
	};
}
