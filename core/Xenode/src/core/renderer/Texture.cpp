#include "pch"
#include "Texture.h"

#include <core/app/GameApplication.h>

#include "gfxapi/OpenGL/OpenGLTexture.h"

namespace Xen {
	Ref<Texture2D> Texture2D::CreateTexture2D(const std::string& textureFilePath, bool flip_on_load)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(textureFilePath, flip_on_load);
		}
		return nullptr;
	}

	Ref<Texture2D> Texture2D::CreateTexture2D(TextureProperties properties, void* data, uint32_t size)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(properties, data, size);
		}
		return nullptr;
	}
	Ref<Texture2D> Texture2D::CreateTexture2D(const Buffer& textureBuffer, TextureBufferType bufferType, TextureProperties properties)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(textureBuffer, bufferType, properties);
		}
		return nullptr;
	}
	void Texture2D::BindTexture(uint32_t id, uint8_t slot)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			OpenGLTexture::BindTextureExtID(id, slot);
		}
	}
	void Texture2D::BindToImageUnit(const Ref<Texture2D>& texture, uint8_t slot, uint8_t mipLevel)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			OpenGLTexture::BindExtTextureToImageUnit(texture, slot, mipLevel);
		}
	}
	void Texture2D::BindToImageUnit(uint32_t textureID, TextureFormat format, uint8_t slot, uint8_t mipLevel)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			OpenGLTexture::BindExtTextureToImageUnit(textureID, format, slot, mipLevel);
		}
	}
	Ref<Texture2D> Texture2D::CreateTexture2D(uint32_t rendererID, TextureProperties properties)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLTexture>(rendererID, properties);
		}
		return nullptr;
	}
}