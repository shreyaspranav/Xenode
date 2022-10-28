#include "pch"
#include "Texture.h"

#include "core/app/GameApplication.h"

#include "gfxapi/OpenGL/OpenGLTexture.h"

namespace Xen {
	Ref<Texture2D> Texture2D::CreateTexture2D(const std::string& textureFilePath)
	{
		switch (GameApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(textureFilePath, 0, 0);
		}
	}
}