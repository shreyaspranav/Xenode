#include "pch"
#include "ShaderAssetImporter.h"

#include <core/app/Timer.h>
#include <core/renderer/ShaderCompiler.h>

#include <project/ProjectManager.h>

namespace Xen
{
	// Global Settings: ---------------------------------------------------------------------------------------------------------------
	const std::string SHADER_LINE_ENDING = "\r\n";
	// --------------------------------------------------------------------------------------------------------------------------------

	// Implementation: ----------------------------------------------------------------------------------------------------------------
	Ref<Asset> ShaderAssetImporter::ImportShaderAsset(AssetMetadata* metadata)
	{
		Ref<Project> currentProject = ProjectManager::GetCurrentProject();
		std::filesystem::path assetPath = currentProject->GetProjectSettings().relAssetDirectory;

		std::filesystem::path completePath = ProjectManager::GetCurrentProjectPath() / assetPath / metadata->relPath;

		XEN_ENGINE_LOG_WARN("Shader file {0} ------------------------------------------------------", completePath.string());
		Vector<std::string> completeShaderCode = ShaderAssetImporter::ReadShaderCode(completePath);
		auto&& shaderSources = ShaderAssetImporter::PreprocessShader(completeShaderCode);

		return ShaderAssetImporter::CompileAndCreateShaderAsset(shaderSources, completePath.filename().string());
	}

	// Private functions: -------------------------------------------------------------------------------------------------------------
	Vector<std::string> ShaderAssetImporter::ReadShaderCode(const std::filesystem::path& completePath)
	{
		// TODO: Measure the performance of this code:
		Timer t;

		std::ifstream inputStream(completePath);
		Vector<std::string> shaderCode;

		std::string currentLine;
		while (!inputStream.eof())
		{
			std::getline(inputStream, currentLine);
			shaderCode.push_back(currentLine);
		}

		t.Stop();

		XEN_ENGINE_LOG_WARN("Shader file read time: {1}", completePath.string(), t.GetElapedTime());
		return shaderCode;
	}

	UnorderedMap<ShaderType, std::string> ShaderAssetImporter::PreprocessShader(const Vector<std::string>& completeShaderCode)
	{
		ShaderType shaderType = ShaderType::None;
		std::stringstream shaderCodeEach;
		bool shaderCodeEachEmpty = true;

		UnorderedMap<ShaderType, std::string> shaders;

		for (auto& stringToken : completeShaderCode)
		{
			if (stringToken.contains("#shadertype"))
			{
				if (!shaderCodeEach.str().empty()) 
				{
					shaders.insert({ shaderType, shaderCodeEach.str() });
					shaderCodeEach.str(std::string());
					shaderCodeEachEmpty = true;
				}

				// Add more types in the future:
				if (stringToken.contains("vertex"))         shaderType = ShaderType::Vertex;
				else if (stringToken.contains("fragment"))  shaderType = ShaderType::Fragment;
				else if (stringToken.contains("geometry"))  shaderType = ShaderType::Geometry;
				
				continue;
			}

			shaderCodeEach << stringToken << "\n";
		}

		if (!shaderCodeEach.str().empty()) 
		{
			shaders.insert({ shaderType, shaderCodeEach.str() });
			shaderCodeEach.str(std::string());
			shaderCodeEachEmpty = true;
		}

		return shaders;
	}

	Ref<Asset> ShaderAssetImporter::CompileAndCreateShaderAsset(const UnorderedMap<ShaderType, std::string>& shaderSources, const std::string& fileName)
	{
		UnorderedMap<ShaderType, Buffer> shaderBinaries;

		for (auto& [shaderType, source] : shaderSources)
		{
			Buffer shaderBinary = ShaderCompiler::CompileShader(source, fileName, {}, shaderType);
			shaderBinaries.insert({ shaderType, shaderBinary });
		}

		Ref<Shader> shaderAsset = Shader::CreateShader(shaderBinaries);
		for (auto& [shaderType, binary] : shaderBinaries)
			binary.Free();

		return shaderAsset;
	}
}