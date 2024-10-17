#pragma once
#include <Core.h>

#include <core/asset/Asset.h>
#include <core/renderer/Shader.h>

namespace Xen
{
	class XEN_API ShaderAssetImporter
	{
	public:
		static Ref<Asset> ImportShaderAsset(AssetMetadata* metadata);
	private:
		static Vector<std::string> ReadShaderCode(const std::filesystem::path& completePath);
		static UnorderedMap<ShaderType, std::string> PreprocessShader(const Vector<std::string>& completeShaderCode);
		static Ref<Asset> CompileAndCreateShaderAsset(const UnorderedMap<ShaderType, std::string>& shaderSources, const std::string& fileName);
	};
}