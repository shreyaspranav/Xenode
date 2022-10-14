#pragma once

#include <Core.h>
#include <glm/glm.hpp>

namespace Xen {

	enum class ShaderType { Source, SpirVBinary };

	class XEN_API Shader
	{
	public:
		virtual void LoadShader() = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual inline uint32_t GetShaderID() = 0; //Temp!

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetInt2(const std::string& name, const glm::vec2& value) = 0;
		virtual void SetInt3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetInt4(const std::string& name, const glm::vec4& value) = 0;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		static Ref<Shader> CreateShader(const std::string& filePath);
		static Ref<Shader> CreateShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type);
	};
}

