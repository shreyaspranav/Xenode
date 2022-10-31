#include "pch"
#include "Texture.h"

#include "core/app/GameApplication.h"

#include "gfxapi/OpenGL/OpenGLTexture.h"

namespace Xen {
	Ref<Texture2D> Texture2D::CreateTexture2D(const std::string& textureFilePath, bool flip_on_load)
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(textureFilePath, flip_on_load);
		}
	}

	Ref<Texture2D> Texture2D::CreateTexture2D(uint32_t width, uint32_t height, void* data, uint32_t size)
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(width, height, data, size);
		}
	}
}