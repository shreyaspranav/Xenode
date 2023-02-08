#include "pch"
#include "Shader.h"

#include "core/app/DesktopApplication.h"

#include "gfxapi/OpenGL/OpenGLShader.h"

namespace Xen {
	Ref<Shader> Shader::CreateShader(const std::string& filePath)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLShader>(filePath);
		}
	}

	Ref<Shader> Shader::CreateShader(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath, ShaderType type)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLShader>(vertexShaderFilePath, fragmentShaderFilePath, type);
		}
	}
}