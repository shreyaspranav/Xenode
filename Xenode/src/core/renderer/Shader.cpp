#include "pch"
#include "Shader.h"

#include "core/app/GameApplication.h"

#include "gfxapi/OpenGL/OpenGLShader.h"

namespace Xen {
	Ref<Shader> Shader::CreateShader(const std::string& filePath)
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLShader>(filePath);
		case GraphicsAPI::XEN_VULKAN_API:
			return nullptr;
		}
	}

	Ref<Shader> Shader::CreateShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type)
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLShader>(vertexShaderFilePath, fragmentShaderFilePath, type);
		case GraphicsAPI::XEN_VULKAN_API:
			return nullptr;
		}
	}
}