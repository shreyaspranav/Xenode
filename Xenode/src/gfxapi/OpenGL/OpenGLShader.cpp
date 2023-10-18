#include "pch"
#include "OpenGLShader.h"

#include "core/app/Log.h"

#include <glad/gl.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/app/Profiler.h"

#include "shaderc/shaderc.hpp"
#include "SHA256.h"
#include <core/app/Timer.h>

namespace Xen {

	static shaderc_shader_kind GLShaderToShaderC(GLenum shaderKind)
	{
		switch (shaderKind)
		{
			case GL_VERTEX_SHADER:		return shaderc_vertex_shader;
			case GL_FRAGMENT_SHADER:	return shaderc_fragment_shader;
			case GL_GEOMETRY_SHADER:	return shaderc_geometry_shader;
		}
		XEN_ENGINE_LOG_ERROR("Unknown Shader Type! ");
		TRIGGER_BREAKPOINT;
	
		return (shaderc_shader_kind)0;
	}

	static const char* GLShaderToString(GLenum shaderKind)
	{
		switch (shaderKind)
		{
		case GL_VERTEX_SHADER:		return "GL_VERTEX_SHADER";
		case GL_FRAGMENT_SHADER:	return "GL_FRAGMENT_SHADER";
		case GL_GEOMETRY_SHADER:	return "GL_GEOMETRY_SHADER";
		}
		XEN_ENGINE_LOG_ERROR("Unknown Shader Type! ");
		TRIGGER_BREAKPOINT;

		return "";
	}

	OpenGLShader::OpenGLShader(const std::string& filePath)
	{
		XEN_PROFILE_FN();

		std::filesystem::path shaderFilePath(filePath);
		m_FileName = shaderFilePath.filename().string();

		std::ifstream stream;
		stream.open(filePath);

		std::vector<std::string> shaderCode;
		std::stringstream shaderCodeStream;
		std::string s;
		while (!stream.eof()) 
		{
			std::getline(stream, s);
			shaderCode.push_back(s);
			shaderCodeStream << s << "\n";
		}
		stream.close();
		
		m_ShaderSrc = PreprocessShaders(shaderCode);
		m_ShaderSrcSHADigest = HashShaderCode(m_ShaderSrc);

		m_ShaderProgramID = glCreateProgram();
	}
	OpenGLShader::OpenGLShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath)
	{
		std::ifstream stream;
		stream.open(vertexShaderFilePath);

		std::stringstream ss;
		while (!stream.eof())
		{
			std::string s;
			std::getline(stream, s);
			ss << s << SHADER_LINE_ENDING;
		}

		m_ShaderSrc.insert({ GL_VERTEX_SHADER, ss.str() });
		stream.close();

		stream.open(fragmentShaderFilePath);
		ss.str(std::string());

		while (!stream.eof())
		{
			std::string s;
			std::getline(stream, s);
			ss << s << SHADER_LINE_ENDING;
		}

		m_ShaderSrc.insert({ GL_FRAGMENT_SHADER, ss.str() });
		m_ShaderSrcSHADigest = HashShaderCode(m_ShaderSrc);

		m_ShaderProgramID = glCreateProgram();
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_ShaderProgramID);

		for (auto& [shaderType, hash] : m_ShaderSrcSHADigest)
			delete[] hash;
	}

	std::unordered_map<GLenum, std::string> Xen::OpenGLShader::PreprocessShaders(const std::vector<std::string>& shaderCode)
	{
		GLenum shaderType = 0;
		std::stringstream shaderCodeEach;
		bool shaderCodeEachEmpty = true;

		std::unordered_map<GLenum, std::string> shaders;

		for (auto& stringToken : shaderCode) 
		{
			if (stringToken.contains("#shadertype")) 
			{
				if (!shaderCodeEach.str().empty()) {
					shaders.insert({ shaderType, shaderCodeEach.str() });
					shaderCodeEach.str(std::string());
					shaderCodeEachEmpty = true;
				}

				if (stringToken.contains("vertex"))
					shaderType = GL_VERTEX_SHADER;
				else if (stringToken.contains("fragment"))
					shaderType = GL_FRAGMENT_SHADER;
				else if (stringToken.contains("geometry"))
					shaderType = GL_GEOMETRY_SHADER;
				continue;
			}
			shaderCodeEach << stringToken << SHADER_LINE_ENDING;
		}

		if (!shaderCodeEach.str().empty()) {
			shaders.insert({ shaderType, shaderCodeEach.str() });
			shaderCodeEach.str(std::string());
			shaderCodeEachEmpty = true;
		}

		return shaders;
	}

	inline std::unordered_map<GLenum, uint8_t*> OpenGLShader::HashShaderCode(const std::unordered_map<GLenum, std::string>& shaderCode)
	{
		std::unordered_map<GLenum, uint8_t*> hashes;

		for (auto&& [shaderType, shaderString] : shaderCode)
		{
			SHA256 shaEngine;
			shaEngine.update(shaderString);
			
			// Generate SHA256 Digest:
			uint8_t* digest = shaEngine.digest();
			hashes.insert({ shaderType, digest });
		}

		return hashes;
	}

	inline void OpenGLShader::LinkShaders(GLenum shaderType, const std::vector<uint32_t>& shaderBinary)
	{
		uint32_t shaderID = glCreateShader(shaderType);
		glShaderBinary(1, &shaderID, GL_SHADER_BINARY_FORMAT_SPIR_V, shaderBinary.data(), shaderBinary.size() * sizeof(uint32_t));
		glSpecializeShader(shaderID, "main", 0, nullptr, nullptr);

		glAttachShader(m_ShaderProgramID, shaderID);
	}

	inline void OpenGLShader::CompileAndLinkShaders(GLenum shaderType,
		const std::string& shaderSource)
	{
		shaderc::Compiler compiler;
		shaderc::CompileOptions compileOptions;

		compileOptions.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);

		// Maybe we need zero level optimisation for debug purposes? : FIND OUT
		compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult shaderCompilationResult =
			compiler.CompileGlslToSpv(shaderSource, GLShaderToShaderC(shaderType), GLShaderToString(shaderType), compileOptions);

		if (shaderCompilationResult.GetNumErrors() > 0)
		{
			XEN_ENGINE_LOG_ERROR("In File {0}, {1}: Compilation failed: {2} Error(s)",
				m_FileName, GLShaderToString(shaderType), shaderCompilationResult.GetNumErrors());

			XEN_ENGINE_LOG_ERROR(shaderCompilationResult.GetErrorMessage());

			// Do Better error handling than to just crash the whole program:
			TRIGGER_BREAKPOINT;
		}
		else {
			XEN_ENGINE_LOG_INFO("{0}: {1} Successfully compiled: {2} Warning(s)",
				m_FileName, GLShaderToString(shaderType), shaderCompilationResult.GetNumWarnings());

			std::vector<uint32_t> shaderBinary = { shaderCompilationResult.begin(), shaderCompilationResult.end() };

			LinkShaders(shaderType, shaderBinary);

			// Maybe do this in a seperate thread
			CacheShaderBinary(m_FileName, shaderType, shaderBinary);
		}
	}

	void OpenGLShader::CacheShaderBinary(const std::string& fileName, GLenum shaderType, const std::vector<uint32_t>& shaderBinary)
	{
		std::string binaryFileExtension;
		std::string shaDigestFileExtension;

		switch (shaderType)
		{
		case GL_VERTEX_SHADER:		binaryFileExtension = ".cached_vs.bin";	shaDigestFileExtension = ".vs.sha256"; break;
		case GL_FRAGMENT_SHADER:	binaryFileExtension = ".cached_fs.bin";	shaDigestFileExtension = ".fs.sha256"; break;
		case GL_GEOMETRY_SHADER:	binaryFileExtension = ".cached_gs.bin";	shaDigestFileExtension = ".gs.sha256"; break;
		}

		if (binaryFileExtension.empty() || shaDigestFileExtension.empty()) {
			XEN_ENGINE_LOG_ERROR("Unknown Shader Type!");
			TRIGGER_BREAKPOINT;
		}

		std::string finalBinaryFileName = cacheDirectory + fileName + binaryFileExtension;
		std::string finalSHAFileName = cacheDirectory + fileName + shaDigestFileExtension;
		std::string sha256Digest = SHA256::toString(m_ShaderSrcSHADigest[shaderType]);
		
		std::ofstream outputStreamBinary(finalBinaryFileName, std::ios::out | std::ios::binary | std::ios::trunc);
		std::ofstream outputStreamText(finalSHAFileName, std::ios::out | std::ios::trunc);

		outputStreamBinary.write(reinterpret_cast<const char*>(shaderBinary.data()), shaderBinary.size() * sizeof(uint32_t));
		outputStreamText.write(sha256Digest.c_str(), sha256Digest.size() * sizeof(char));

		outputStreamBinary.close();
		outputStreamText.close();
	}

	inline bool OpenGLShader::DoesShaderBinaryFileExist(const std::string& fileName, GLenum shaderType)
	{
		std::string binaryFileExtension;

		switch (shaderType)
		{
		case GL_VERTEX_SHADER:		binaryFileExtension = ".cached_vs.bin";	break;
		case GL_FRAGMENT_SHADER:	binaryFileExtension = ".cached_fs.bin";	break;
		case GL_GEOMETRY_SHADER:	binaryFileExtension = ".cached_gs.bin";	break;
		}

		if (!std::filesystem::exists(std::filesystem::path(cacheDirectory) / (fileName + binaryFileExtension)))
			return false;

		return true;
	}

	std::string OpenGLShader::ReadShaderHashCacheFromFile(const std::string& fileName, GLenum shaderType)
	{
		std::string shaDigestFileExtension;

		switch (shaderType)
		{
		case GL_VERTEX_SHADER:		shaDigestFileExtension = ".vs.sha256"; break;
		case GL_FRAGMENT_SHADER:	shaDigestFileExtension = ".fs.sha256"; break;
		case GL_GEOMETRY_SHADER:	shaDigestFileExtension = ".gs.sha256"; break;
		}

		if (!std::filesystem::exists(std::filesystem::path(cacheDirectory) / (fileName + shaDigestFileExtension)))
			return "";

		std::string fullFileName = cacheDirectory + fileName + shaDigestFileExtension;
		std::ifstream inputFileStream(fullFileName, std::ios::in);

		// Since the SHA256 Digest file contains only one line:
		std::string to_return;
		std::getline(inputFileStream, to_return);
		return to_return;
	}

	inline std::vector<uint32_t> OpenGLShader::ReadShaderBinary(const std::string& fileName, GLenum shaderType)
	{
		std::string binaryFileExtension;

		switch (shaderType)
		{
		case GL_VERTEX_SHADER:		binaryFileExtension = ".cached_vs.bin";	break;
		case GL_FRAGMENT_SHADER:	binaryFileExtension = ".cached_fs.bin";	break;
		case GL_GEOMETRY_SHADER:	binaryFileExtension = ".cached_gs.bin";	break;
		}

		if (!std::filesystem::exists(std::filesystem::path(cacheDirectory) / (fileName + binaryFileExtension)))
			return std::vector<uint32_t>();

		std::string fullFileName = cacheDirectory + fileName + binaryFileExtension;
		std::ifstream inputFileStream(fullFileName, std::ios::in | std::ios::binary);

		// Get the size of the binary file
		uint64_t size = std::filesystem::file_size(std::filesystem::path(fullFileName));

		std::vector<uint32_t> to_return;
		to_return.resize(size / sizeof(uint32_t));

		inputFileStream.read((char*)to_return.data(), size);

		return to_return;
	}

	void OpenGLShader::LoadShader(const VertexBufferLayout& layout)
	{
		XEN_PROFILE_FN();

		Timer t;

		// Create Cache directory if needed, if it doesn't exist, compile shaders:
		if (!std::filesystem::exists(std::filesystem::path(cacheDirectory)))
		{
			std::filesystem::create_directories(std::filesystem::path(cacheDirectory));

			for (auto& [shaderType, shaderSrc] : m_ShaderSrc)
				CompileAndLinkShaders(shaderType, shaderSrc);
		}

		else {
			// Or else find the sha256 digest of each and compare it with the existing one, recompile only the nessessary shaders

			for (auto& [shaderType, shaderSrc] : m_ShaderSrc)
			{
				bool isSHA256DigestEqual = ReadShaderHashCacheFromFile(m_FileName, shaderType) == SHA256::toString(m_ShaderSrcSHADigest[shaderType]);
				bool doesBinaryFileExist = DoesShaderBinaryFileExist(m_FileName, shaderType);

				if (isSHA256DigestEqual && doesBinaryFileExist) {
					std::vector<uint32_t> shaderBinary = ReadShaderBinary(m_FileName, shaderType);		
					LinkShaders(shaderType, shaderBinary);
				}

				else
					CompileAndLinkShaders(shaderType, shaderSrc);
			}
		}

		int success;
		char infoLog[512];

		glLinkProgram(m_ShaderProgramID);
		glGetProgramiv(m_ShaderProgramID, GL_LINK_STATUS, &success);

		t.Stop();
		XEN_ENGINE_LOG_WARN("{0}: Shader creation took {1}ms", m_FileName, t.GetElapedTime() / 1000.0f);

		if (success == GL_FALSE)
		{
			glGetProgramInfoLog(m_ShaderProgramID, 512, NULL, infoLog);
			XEN_ENGINE_LOG_ERROR("Failed to Link Shader Program!");
			XEN_ENGINE_LOG_ERROR(infoLog);
			TRIGGER_BREAKPOINT;
		}
	}
	
	inline void OpenGLShader::Bind() const		{ XEN_PROFILE_FN(); glUseProgram(m_ShaderProgramID); }
	inline void OpenGLShader::Unbind() const	{ glUseProgram(0); }

	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		if (m_Uniforms.contains(name))
		{
			glUniform1f(m_Uniforms[name], value);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform1f(m_Uniforms[name], value);
	}
	void OpenGLShader::SetFloat2(const std::string& name, const Vec2& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform2f(m_Uniforms[name], value.x, value.y);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform2f(m_Uniforms[name], value.x, value.y);
	}
	void OpenGLShader::SetFloat3(const std::string& name, const Vec3& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform3f(m_Uniforms[name], value.x, value.y, value.z);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform3f(m_Uniforms[name], value.x, value.y, value.z);
	}
	void OpenGLShader::SetFloat4(const std::string& name, const Vec4& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform4f(m_Uniforms[name], value.x, value.y, value.z, value.w);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform4f(m_Uniforms[name], value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetIntArray(const std::string& name, int* array, uint32_t count)
	{
		if (m_Uniforms.contains(name))
		{
			glUniform1iv(m_Uniforms[name], count, array);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform1iv(m_Uniforms[name], count, array);

	}
	void OpenGLShader::SetInt(const std::string& name, int value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform1i(m_Uniforms[name], value);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform1i(m_Uniforms[name], value);
	}
	void OpenGLShader::SetInt2(const std::string& name, const Vec2& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform2i(m_Uniforms[name], (int)value.x, (int)value.y);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform2i(m_Uniforms[name], (int)value.x, (int)value.y);
	}
	void OpenGLShader::SetInt3(const std::string& name, const Vec3& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform3i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform3i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z);
	}
	void OpenGLShader::SetInt4(const std::string& name, const Vec4& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniform4i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z, (int)value.w);
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniform4i(m_Uniforms[name], (int)value.x, (int)value.y, (int)value.z, (int)value.w);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value) 
	{
		if (m_Uniforms.contains(name))
		{
			glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
			return;
		}

		int location = glGetUniformLocation(m_ShaderProgramID, name.c_str());
		if (location == -1)
			XEN_ENGINE_LOG_ERROR("'{0}' is not a valid uniform", name);

		m_Uniforms[name] = location;
		glUniformMatrix4fv(m_Uniforms[name], 1, GL_FALSE, glm::value_ptr(value));
	}
}