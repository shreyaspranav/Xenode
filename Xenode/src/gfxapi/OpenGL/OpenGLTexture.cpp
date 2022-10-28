#include "pch"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <core/app/Log.h>

#include <glad/gl.h>

namespace Xen {
	OpenGLTexture::OpenGLTexture(const std::string& textureFilePath, uint8_t texture_slot, bool flip_on_load) : m_TextureSlot(texture_slot), m_FilePath(textureFilePath)
	{
		int width, height, channels;
		m_TextureData = stbi_load(textureFilePath.c_str(), &width, &height, &channels, 0);

		if (m_TextureData == NULL)
		{
			XEN_ENGINE_LOG_ERROR("Failed To read Texture File!");
			TRIGGER_BREAKPOINT;
		}

		m_Width = width;
		m_Height = height;
		m_Channels = channels;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, GL_RGB8, m_Width, m_Height);
	}
	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}
	void OpenGLTexture::LoadTexture()
	{
		if (m_T_FilterMode == TextureFilterMode::Linear)
		{
			glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else if (m_T_FilterMode == TextureFilterMode::Nearest)
		{
			glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}

		switch (m_T_WrapMode)
		{
			case TextureWrapMode::Repeat:
			{
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
				break;
			}

			case TextureWrapMode::MirroredRepeat:
			{
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				break;
			}

			case TextureWrapMode::ClampToBorder:
			{
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				break;
			}

			case TextureWrapMode::ClampToEdge:
			{
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				break;
			}

		}

		if(m_Channels == 4)
			glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RED, GL_UNSIGNED_BYTE, (const void*)m_TextureData);
		else if (m_Channels == 3)
			glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, (const void*)m_TextureData);

		if (!store_in_cpu_mem)
			stbi_image_free(m_TextureData);
	}
	void OpenGLTexture::SetTextureWrapMode(TextureWrapMode mode)
	{
		m_T_WrapMode = mode;
	}
	void OpenGLTexture::SetTextureFilterMode(TextureFilterMode mode)
	{
		m_T_FilterMode = mode;
	}

	void OpenGLTexture::Bind() const
	{
		glBindTextureUnit(m_TextureSlot, m_TextureID);
	}
}