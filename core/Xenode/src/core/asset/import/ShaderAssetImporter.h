#pragma once
#include <Core.h>

#include <core/asset/AssetMetadata.h>
#include <core/renderer/Shader.h>

namespace Xen
{
	class XEN_API ShaderAssetImporter
	{
	public:
		static Vector<std::byte> ImportShaderAsset(AssetMetadata* metadata);
		static Ref<Asset> LoadShaderAsset(const Vector<std::byte>& buffer, AssetMetadata* metadata);
	private:
		static Vector<std::string> ReadShaderCode(const std::filesystem::path& completePath);
		static UnorderedMap<ShaderType, std::string> PreprocessShader(const Vector<std::string>& completeShaderCode);
		static Vector<std::byte> CompileShaderAsset(const UnorderedMap<ShaderType, std::string>& shaderSources, const std::string& fileName, AssetMetadata* metadata);
	};
}