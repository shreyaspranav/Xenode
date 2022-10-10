#pragma once

#include <Core.h>

namespace Xen {

	enum class ShaderType { Source, SpirVBinary };

	class XEN_API Shader
	{
	public:
		virtual void LoadShader() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual inline uint32_t GetShaderID() = 0; //Temp!

		static Ref<Shader> CreateShader(const std::string& filePath);
		static Ref<Shader> CreateShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type);
	};
}

