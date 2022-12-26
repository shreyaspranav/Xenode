#pragma once

#include <Core.h>
#include <glm/glm.hpp>
#include "Structs.h"

#include "Buffer.h"

namespace Xen {

	enum class ShaderType { Source, SpirVBinary };

	class XEN_API Shader
	{
	public:
		virtual void LoadShader(const BufferLayout& layout) = 0;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual inline uint32_t GetShaderID() = 0; //Temp!

		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat2(const std::string& name, const Vec2& value) = 0;
		virtual void SetFloat3(const std::string& name, const Vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const Vec4& value) = 0;

		virtual void SetIntArray(const std::string& name, int* array, uint32_t count) = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetInt2(const std::string& name, const Vec2& value) = 0;
		virtual void SetInt3(const std::string& name, const Vec3& value) = 0;
		virtual void SetInt4(const std::string& name, const Vec4& value) = 0;

		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

		static Ref<Shader> CreateShader(const std::string& filePath);
		static Ref<Shader> CreateShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type);
	};

	// Class Static For now.. Just to simplify things
	class XEN_API ShaderLib
	{
	private:
		inline static std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	public:
		static void AddShader(const std::string& name, const Ref<Shader>& shader)	{ m_Shaders[name] = shader; }
		static void DeleteShader(const std::string& name)							{ m_Shaders.erase(name); }

		static const Ref<Shader>& GetShader(const std::string& name)				{ return m_Shaders[name]; }

		static inline uint32_t GetSize()											{ return m_Shaders.size(); }
	};
}

