#include "pch"
#include "Texture.h"

#include "core/app/DesktopApplication.h"

#include "gfxapi/OpenGL/OpenGLTexture.h"

namespace Xen {
	Ref<Texture2D> Texture2D::CreateTexture2D(const std::string& textureFilePath, bool flip_on_load)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(textureFilePath, flip_on_load);
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::CreateTexture2D(uint32_t width, uint32_t height, void* data, uint32_t size)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(width, height, data, size);
		}
		return nullptr;
	}
}