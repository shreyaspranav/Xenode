#include "pch"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <core/app/Log.h>

#include <glad/gl.h>

#include "OpenGLContext.h"

namespace Xen {

	static GLenum ToGLTextureType(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::G8:	
		case TextureFormat::GA8:
		case TextureFormat::RGB8:
		case TextureFormat::RGBA8:
			return GL_UNSIGNED_BYTE;
		case TextureFormat::G16:
		case TextureFormat::GA16:
		case TextureFormat::RGB16:
		case TextureFormat::RGBA16:
			return GL_UNSIGNED_SHORT;
		case TextureFormat::RGB16F:	
		case TextureFormat::RGB32F:	
		case TextureFormat::RGBA16F:
		case TextureFormat::RGBA32F:
			return GL_FLOAT;
		}

		XEN_ENGINE_LOG_ERROR("Unknown TextureFormat type!");
		TRIGGER_BREAKPOINT;

		return (GLenum)0;
	}

	static GLenum ToGLInternalTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::G8:				return GL_R8;
		case TextureFormat::G16:			return GL_R16;
		case TextureFormat::GA8:			return GL_RG8;
		case TextureFormat::GA16:			return GL_RG16;
		case TextureFormat::RGB8:			return GL_RGB8;
		case TextureFormat::RGB16:			return GL_RGB16;
		case TextureFormat::RGB16F:			return GL_RGB16F;
		case TextureFormat::RGB32F:			return GL_RGB32F;
		case TextureFormat::RGBA8:			return GL_RGBA8;
		case TextureFormat::RGBA16:			return GL_RGBA16;
		case TextureFormat::RGBA16F:		return GL_RGBA16F;
		case TextureFormat::RGBA32F:		return GL_RGBA32F;
		}


		XEN_ENGINE_LOG_ERROR("Unknown TextureFormat type!");
		TRIGGER_BREAKPOINT;

		return (GLenum)0;

	}

	static GLenum ToGLTextureFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::G8:	
		case TextureFormat::G16:
			return GL_RED;
		case TextureFormat::GA8:
		case TextureFormat::GA16:
			return GL_RG;
		case TextureFormat::RGB8:
		case TextureFormat::RGB16:
		case TextureFormat::RGB16F:
		case TextureFormat::RGB32F:
			return GL_RGB;
		case TextureFormat::RGBA8:	
		case TextureFormat::RGBA16:	
		case TextureFormat::RGBA16F:
		case TextureFormat::RGBA32F:
			return GL_RGBA;
		}

		XEN_ENGINE_LOG_ERROR("Unknown TextureFormat type!");
		TRIGGER_BREAKPOINT;

		return (GLenum)0;
	}

	static GLenum ToGLFilterMode(TextureFilterMode mode)
	{
		switch (mode)
		{
			case Xen::TextureFilterMode::Linear:	return GL_LINEAR;
			case Xen::TextureFilterMode::Nearest:	return GL_NEAREST;
		}

		XEN_ENGINE_LOG_ERROR("Unknown TextureFilter type!");
		TRIGGER_BREAKPOINT;

		return (GLenum)0;


		XEN_ENGINE_LOG_ERROR("Unknown TextureFilter type!");
		TRIGGER_BREAKPOINT;

		return (GLenum)0;

	}

	static GLenum ToGLWrapMode(TextureWrapMode mode)
	{
		switch (mode)
		{
			case Xen::TextureWrapMode::Repeat:				return GL_REPEAT;
			case Xen::TextureWrapMode::MirroredRepeat:		return GL_MIRRORED_REPEAT;
			case Xen::TextureWrapMode::ClampToEdge:			return GL_CLAMP_TO_EDGE;
			case Xen::TextureWrapMode::ClampToBorder:		return GL_CLAMP_TO_BORDER;
		}

		XEN_ENGINE_LOG_ERROR("Unknown TextureWrap type!");
		TRIGGER_BREAKPOINT;

		return (GLenum)0;
	}

	static void SetTexFilterMode(uint32_t textureID, GLenum mode)
	{
		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, mode);
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, mode);
	}

	static void SetTexWrapMode(uint32_t textureID, GLenum mode)
	{
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, mode);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, mode);
	}

	OpenGLTexture::OpenGLTexture(const std::string& textureFilePath, bool flip_on_load) : m_FilePath(textureFilePath)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load((int)flip_on_load);
		stbi_info(textureFilePath.c_str(), &width, &height, &channels);

		std::string failureReason(stbi_failure_reason());

		m_TextureProperties.width = width;
		m_TextureProperties.height = height;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);

		// Finding out about the type of texture:
		bool is_16_bit = stbi_is_16_bit(textureFilePath.c_str());

		switch (channels)
		{
		case 1:
			if (is_16_bit)
				m_TextureProperties.format = TextureFormat::G16;
			else
				m_TextureProperties.format = TextureFormat::G8;
			break;
		case 2:
			if (is_16_bit)
				m_TextureProperties.format = TextureFormat::GA16;
			else
				m_TextureProperties.format = TextureFormat::GA8;
			break;
		case 3:
			if (is_16_bit)
				m_TextureProperties.format = TextureFormat::RGB16;
			else
				m_TextureProperties.format = TextureFormat::RGB8;
			break;
		case 4:
			if (is_16_bit)
				m_TextureProperties.format = TextureFormat::RGBA16;
			else
				m_TextureProperties.format = TextureFormat::RGBA8;
			break;
		default:
			break;
		}

		glTextureStorage2D(m_TextureID, 1, ToGLInternalTextureFormat(m_TextureProperties.format), m_TextureProperties.width, m_TextureProperties.height);

	}

	OpenGLTexture::OpenGLTexture(TextureProperties properties, void* data, uint32_t size)
	{
		m_TextureProperties = properties;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, ToGLInternalTextureFormat(m_TextureProperties.format), m_TextureProperties.width, m_TextureProperties.height);

		SetTexFilterMode(m_TextureID, ToGLFilterMode(m_T_FilterMode));
		SetTexWrapMode(m_TextureID, ToGLWrapMode(m_T_WrapMode));

		if(data != nullptr)
			glTextureSubImage2D(m_TextureID, 0, 0, 0, m_TextureProperties.width, m_TextureProperties.height, 
				ToGLTextureFormat(m_TextureProperties.format), 
				ToGLTextureType(m_TextureProperties.format), data);

	}

	OpenGLTexture::OpenGLTexture(uint32_t rendererID)
	{
		m_TextureID = rendererID;
	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}
	void OpenGLTexture::LoadTexture()
	{
		SetTexFilterMode(m_TextureID, ToGLFilterMode(m_T_FilterMode));
		SetTexWrapMode(m_TextureID, ToGLWrapMode(m_T_WrapMode));

		void* textureData;
		int width, height, channels;

		if (stbi_is_16_bit(m_FilePath.c_str()))
			textureData = stbi_load_16(m_FilePath.c_str(), &width, &height, &channels, 0);
		else
			textureData = stbi_load(m_FilePath.c_str(), &width, &height, &channels, 0);

		if (textureData == NULL)
		{
			XEN_ENGINE_LOG_ERROR("Failed To read Texture File!");
			TRIGGER_BREAKPOINT;
		}
		
		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_TextureProperties.width, m_TextureProperties.height,
			ToGLTextureFormat(m_TextureProperties.format),
			ToGLTextureType(m_TextureProperties.format), textureData);

		stbi_image_free(textureData);
	}
	void OpenGLTexture::SetTextureWrapMode(TextureWrapMode mode)
	{ 
		m_T_WrapMode = mode;
		SetTexWrapMode(m_TextureID, ToGLWrapMode(mode));
	}
	void OpenGLTexture::SetTextureFilterMode(TextureFilterMode mode)
	{ 
		m_T_FilterMode = mode; 
		SetTexFilterMode(m_TextureID, ToGLFilterMode(mode));
	}

	void OpenGLTexture::Bind(uint8_t slot) const
	{
		glBindTextureUnit(slot, m_TextureID);
	}
	void OpenGLTexture::BindTextureExtID(uint32_t id, uint8_t slot)
	{
		glBindTextureUnit(slot, id);
	}
	void OpenGLTexture::BindExtTextureToImageUnit(const Ref<Texture2D> texture, uint8_t slot)
	{
		glBindImageTexture(0, texture->GetNativeTextureID(), 0, GL_FALSE, 0, 
			GL_READ_WRITE, ToGLInternalTextureFormat(texture->GetTextureProperties().format));
	}
}