#include "pch"
#include "AssetPackManager.h"

#include <project/ProjectManager.h>

namespace Xen
{
#if 0
	void AssetPackManager::CreateAssetPack(const AssetMetadataRegistry& metadataRegistry)
	{
		std::filesystem::path projectPath = ProjectManager::GetCurrentProjectPath();
		std::string projectName = ProjectManager::GetCurrentProject()->GetProjectProperties().name;

		// Create a directory called 'pack' where the asset pack will be stored.
		std::filesystem::path assetPackPath = projectPath / ProjectManager::GetCurrentProject()->GetProjectSettings().relBuildDirectory / "path";
		std::filesystem::create_directory(assetPackPath);

		std::filesystem::path metadataFilePath = assetPackPath / (projectName + ".xam");
		std::filesystem::path assetBinaryFile = assetPackPath / (projectName + ".xapk");

		// Create the metadata file first.
		std::ofstream outputFileStream(metadataFilePath);
	}
#endif
}