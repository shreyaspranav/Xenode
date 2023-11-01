#pragma once

#include "core/renderer/Shader.h" 

namespace Xen {
	typedef unsigned int GLenum;

	class OpenGLShader : public Shader
	{
	public:
		friend class OpenGLComputeShader;

		OpenGLShader(const std::string& filePath);
		OpenGLShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath);

		virtual ~OpenGLShader();

		void LoadShader(const VertexBufferLayout& layout) override;

		inline uint32_t GetShaderID() override { return m_ShaderProgramID; }

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
		std::unordered_map<GLenum, std::string> PreprocessShaders(const std::vector<std::string>& shaderCode);
		std::unordered_map<GLenum, uint8_t*> HashShaderCode(const std::unordered_map<GLenum, std::string>& shaderCode);

		inline void LinkShaders(GLenum shaderType, const std::vector<uint32_t>& shaderBinary);
		inline void CompileAndLinkShaders(GLenum shaderType, const std::string& shaderSource);

		void CacheShaderBinary(const std::string& fileName, GLenum shaderType, const std::vector<uint32_t>& shaderBinary);
		inline bool DoesShaderBinaryFileExist(const std::string& fileName, GLenum shaderType);
		inline std::vector<uint32_t> ReadShaderBinary(const std::string& fileName, GLenum shaderType);

		std::string ReadShaderHashCacheFromFile(const std::string& fileName, GLenum shaderType);

	private:
		uint32_t m_ShaderProgramID;
		std::unordered_map<GLenum, std::string> m_ShaderSrc;
		std::unordered_map<GLenum, uint8_t*> m_ShaderSrcSHADigest;

		std::unordered_map<std::string, uint32_t> m_Uniforms;
		std::string m_FileName;

		const std::string SHADER_LINE_ENDING = "\r\n";

		// Combine this with the asset pipline or the project system
		std::string cacheDirectory = "assets/.cache/";
	};

	class OpenGLComputeShader : public ComputeShader
	{
	public:
		OpenGLComputeShader(const std::string& filePath);
		virtual ~OpenGLComputeShader();

		void LoadShader() override;

		void DispatchCompute(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) override;
	private:
		uint32_t m_ShaderProgramID;

		std::string m_ShaderSrc;
		const std::string SHADER_LINE_ENDING = "\r\n";

		// Combine this with the asset pipline or the project system
		std::string cacheDirectory = "assets/.cache/";
	};
}

