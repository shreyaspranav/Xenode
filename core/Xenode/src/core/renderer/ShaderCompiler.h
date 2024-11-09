#pragma once
#include <Core.h>
#include <core/app/GameApplication.h>

#include "Shader.h"

namespace Xen
{
	// Shader compilation process:
	// 
	//                 shaderc        spirv-cross                    (1)
	// Vulkan GLSL source -> SPIR V Binary -> GFX API specific source -> Final Shader binary
	// (1) -> shaderc for OpenGL and Vulkan, DXC for D3D
	class ShaderCompiler
	{
	public:
		static Buffer CompileShader(
			const std::string& shaderSource,
			const std::string& fileName,
			const Vector <Pair<std::string, std::string>>& defines,
			ShaderType type);

	private:
		static Buffer CompileToVulkanSpirV(
			const std::string& shaderSource,
			const std::string& fileName,
			const Vector <Pair<std::string, std::string>>& defines,
			ShaderType type);

		static std::string GetAPISpecificSource(
			Buffer spirv,
			ShaderType type,
			GraphicsAPI targetAPI,
			const std::string& fileName
		);

		static std::string ReflectSpirVBinary(
			Buffer spirv, 
			ShaderType type, 
			GraphicsAPI targetAPI, 
			const std::string& fileName);

		static Buffer GetFinalShaderBinary(
			const std::string& source, 
			ShaderType type, 
			GraphicsAPI targetAPI, 
			const std::string& fileName);
	};
}