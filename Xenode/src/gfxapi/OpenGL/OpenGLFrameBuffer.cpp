#include "pch"
#include "OpenGLFrameBuffer.h"

#include <glad/gl.h>

#include <core/app/Log.h>
#include "OpenGLContext.h"

namespace Xen {
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpec& spec) : m_Spec(spec)
	{
		Invalidate();
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		if(OpenGLContext::GetOpenGLVersion() == OpenGLVersion::XEN_OPENGL_API_2_0)
			glDeleteFramebuffersEXT(1, &m_FrameBufferID);
		else
			glDeleteFramebuffers(1, &m_FrameBufferID);
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		if (!frame_buffer_created)
		{

			if (OpenGLContext::GetOpenGLVersion() == OpenGLVersion::XEN_OPENGL_API_2_0)
			{
				glGenFramebuffersEXT(1, &m_FrameBufferID);
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FrameBufferID);
			}
			else
			{
				glGenFramebuffers(1, &m_FrameBufferID);
				glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
			}
			
			if (OpenGLContext::GetOpenGLVersion() >= OpenGLVersion::XEN_OPENGL_API_4_5)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentT);
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentT);
				glTextureStorage2D(m_ColorAttachmentT, 1, GL_RGB8, m_Spec.width, m_Spec.height);

				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // was TexParameter
				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


				glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentT);
				glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentT);
				// glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Spec.width, m_Spec.height); <-- Was this
				glTextureStorage2D(m_DepthAttachmentT, 1, GL_DEPTH24_STENCIL8, m_Spec.width, m_Spec.height);

				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // was TexParameter
				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTextureParameteri(m_ColorAttachmentT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glBindTexture(GL_TEXTURE_2D, 0);
			}

			else
			{
				glGenTextures(1, &m_ColorAttachmentT);
				glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentT);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Spec.width, m_Spec.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


				glGenTextures(1, &m_DepthAttachmentT);
				glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentT);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Spec.width, m_Spec.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			if (OpenGLContext::GetOpenGLVersion() == OpenGLVersion::XEN_OPENGL_API_2_0)
			{
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentT, 0);
				glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentT, 0);

				if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
				{
					XEN_ENGINE_LOG_ERROR("FrameBuffer is imcomplete!");
					TRIGGER_BREAKPOINT;
				}
				glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			}
			else
			{
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentT, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentT, 0);

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					XEN_ENGINE_LOG_ERROR("FrameBuffer is imcomplete!");
					TRIGGER_BREAKPOINT;
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			frame_buffer_created = 1;
		}
		else {

			if (OpenGLContext::GetOpenGLVersion() >= OpenGLVersion::XEN_OPENGL_API_4_5)
			{
				glTextureStorage2D(m_ColorAttachmentT, 1, GL_DEPTH24_STENCIL8, m_Spec.width, m_Spec.height);
				glTextureStorage2D(m_ColorAttachmentT, 1, GL_RGB8, m_Spec.width, m_Spec.height);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentT, 0);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentT, 0);
			}

			//glTextureStorage2D(m_ColorAttachmentT, 1, GL_RGB8, m_Spec.width, m_Spec.height);
			//glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentT);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Spec.width, m_Spec.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			//glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentT, 0);
			//glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.width = width;
		m_Spec.height = height;
		OpenGLFrameBuffer::Invalidate();
	}

	void OpenGLFrameBuffer::Bind()
	{
		if (OpenGLContext::GetOpenGLVersion() == OpenGLVersion::XEN_OPENGL_API_2_0)
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_FrameBufferID);
		else
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
	}
	void OpenGLFrameBuffer::Unbind()
	{
		if (OpenGLContext::GetOpenGLVersion() == OpenGLVersion::XEN_OPENGL_API_2_0)
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	uint32_t OpenGLFrameBuffer::GetColorAttachmentRendererID() const
	{
		return m_ColorAttachmentT;
	}
}