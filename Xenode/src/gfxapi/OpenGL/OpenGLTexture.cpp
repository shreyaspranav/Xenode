#include "pch"
#include "OpenGLTexture.h"

#include "stb_image.h"
#include <core/app/Log.h>

#include <glad/gl.h>

#include "OpenGLContext.h"

namespace Xen {
	OpenGLTexture::OpenGLTexture(const std::string& textureFilePath, bool flip_on_load) : m_FilePath(textureFilePath)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load((int)flip_on_load);
		m_TextureData = stbi_load(textureFilePath.c_str(), &width, &height, &channels, 0);

		if (m_TextureData == NULL)
		{
			XEN_ENGINE_LOG_ERROR("Failed To read Texture File!");
			TRIGGER_BREAKPOINT;
		}

		m_Width = width;
		m_Height = height;
		m_Channels = channels;

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

		if (OpenGLContext::GetOpenGLVersion() >= OpenGLVersion::XEN_OPENGL_API_4_5)
			glTextureStorage2D(m_TextureID, 1, GL_RGBA8, m_Width, m_Height);

		else
		{
			if (m_Channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			else if(m_Channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		}


	}

	OpenGLTexture::OpenGLTexture(uint32_t width, uint32_t height, void* data, uint32_t size)
	{
		m_Width = width;
		m_Height = height;
		m_Channels = 4;
		//m_TextureData = (uint8_t*)data;  // Dangerous?

		if (OpenGLContext::GetOpenGLVersion() >= OpenGLVersion::XEN_OPENGL_API_4_5)
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
			glTextureStorage2D(m_TextureID, 1, GL_RGBA8, m_Width, m_Height);
			glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)data);
		}
		else
		{
			glGenTextures(1, &m_TextureID);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

	}

	OpenGLTexture::~OpenGLTexture()
	{
		glDeleteTextures(1, &m_TextureID);
	}
	void OpenGLTexture::LoadTexture()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		if (OpenGLContext::GetOpenGLVersion() >= OpenGLVersion::XEN_OPENGL_API_4_5)
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

			if (m_Channels == 4)
				glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)m_TextureData);
			else if (m_Channels == 3)
				glTextureSubImage2D(m_TextureID, 0, 0, 0, m_Width, m_Height, GL_RGB, GL_UNSIGNED_BYTE, (const void*)m_TextureData);
		}

		else 
		{
			if (m_T_FilterMode == TextureFilterMode::Linear)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else if (m_T_FilterMode == TextureFilterMode::Nearest)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}

			switch (m_T_WrapMode)
			{
				case TextureWrapMode::Repeat:
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					break;
				}

				case TextureWrapMode::MirroredRepeat:
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
					break;
				}

				case TextureWrapMode::ClampToBorder:
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					break;
				}

				case TextureWrapMode::ClampToEdge:
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					break;
				}
			}

			if (m_Channels == 4)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureData);
			else if (m_Channels == 3)
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_TextureData);
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		if (!store_in_cpu_mem)
			stbi_image_free(m_TextureData);
	}
	void OpenGLTexture::SetTextureWrapMode(TextureWrapMode mode)		{ m_T_WrapMode = mode; }
	void OpenGLTexture::SetTextureFilterMode(TextureFilterMode mode)	{ m_T_FilterMode = mode; }

	void OpenGLTexture::Bind(uint8_t slot) const
	{
		if (OpenGLContext::GetOpenGLVersion() >= OpenGLVersion::XEN_OPENGL_API_4_5)
			glBindTextureUnit(slot, m_TextureID);
		else 
		{
			glActiveTexture(GL_TEXTURE0 + slot);
			glBindTexture(GL_TEXTURE_2D, m_TextureID);
		}
	}
}