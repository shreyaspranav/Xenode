#pragma once

#include "core/renderer/Shader.h" 

namespace Xen {
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type);

		virtual ~OpenGLShader();

		void LoadShader(const VertexBufferLayout& layout) override;

		inline uint32_t GetShaderID() override { return m_ShaderID; }

		void SetFloat(const std::string& name, float value) override;
		void SetFloat2(const std::string& name, const Vec2& value) override;
		void SetFloat3(const std::string& name, const Vec3& value) override;
		void SetFloat4(const std::string& name, const Vec4& value) override;

		void SetIntArray(const std::string& name, int* array, uint32_t count) override;

		void SetInt(const std::string& name, int value) override;
		void SetInt2(const std::string& name, const Vec2& value) override;
		void SetInt3(const std::string& name, const Vec3& value) override;
		void SetInt4(const std::string& name, const Vec4& value) override;

		void SetMat4(const std::string& name, const glm::mat4& value) override;

		inline void Bind() const override;
		inline void Unbind() const override;

	private:
		uint32_t m_ShaderID;

		std::string vertexShaderSrc;
		std::string fragmentShaderSrc;

		std::unordered_map<std::string, uint32_t> m_Uniforms;
	};
}

