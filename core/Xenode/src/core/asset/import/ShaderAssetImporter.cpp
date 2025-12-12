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
	Vector<std::byte> ShaderAssetImporter::ImportShaderAsset(AssetMetadata* metadata)
	{
		Ref<Project> currentProject = ProjectManager::GetCurrentProject();
		std::filesystem::path assetPath = currentProject->GetProjectSettings().relAssetDirectory;

		// This shouldn't happen in runtime!
		EditorAssetMetadata* editorAssetMetadata = (EditorAssetMetadata*)metadata;

		std::filesystem::path completePath = ProjectManager::GetCurrentProjectPath() / assetPath / editorAssetMetadata->relPath;

		XEN_ENGINE_LOG_WARN("Shader file {0} ------------------------------------------------------", completePath.string());
		Vector<std::string> completeShaderCode = ShaderAssetImporter::ReadShaderCode(completePath);
		auto&& shaderSources = ShaderAssetImporter::PreprocessShader(completeShaderCode);

		return ShaderAssetImporter::CompileShaderAsset(shaderSources, completePath.filename().string(), editorAssetMetadata);
	}

	Ref<Asset> ShaderAssetImporter::LoadShaderAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata)
	{
		ShaderMetadata& shaderMetadata = std::get<ShaderMetadata>(metadata->specific);
		
		UnorderedMap<ShaderType, Vector<std::byte>> shaders;
		for (auto&& [shaderType, span] : shaderMetadata)
		{
			auto&& start = span.offset;
			auto&& end = span.offset + span.size;

			Vector<std::byte> bin(span.size);
			std::memcpy(bin.data(), buffer.data() + span.offset, span.size);

			shaders.insert({ shaderType, bin });
		}

		Ref<Shader> shaderAsset = Shader::CreateShader(shaders);
		return shaderAsset;
	}

	// Private functions: -------------------------------------------------------------------------------------------------------------
	Vector<std::string> ShaderAssetImporter::ReadShaderCode(const std::filesystem::path& completePath)
	{
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

	Vector<std::byte> ShaderAssetImporter::CompileShaderAsset(const UnorderedMap<ShaderType, std::string>& shaderSources, const std::string& fileName, AssetMetadata* metadata)
	{
		Vector<std::byte> buffer;
		ShaderMetadata shaderMetadata = ShaderMetadata();

		for (auto& [shaderType, source] : shaderSources)
		{
			Size offset = buffer.size();

			Vector<std::byte> shaderBinary = ShaderCompiler::CompileShader(source, fileName, {}, shaderType);
			buffer.insert(buffer.end(), shaderBinary.begin(), shaderBinary.end());

			shaderMetadata.insert({ shaderType, { offset, shaderBinary.size() }});
		}
		metadata->size = buffer.size();
		metadata->specific = shaderMetadata;

		return buffer;
	}
}