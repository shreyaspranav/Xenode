#include "pch"
#include "OpenGLFrameBuffer.h"

#include <glad/gl.h>

#include <core/app/Log.h>

namespace Xen {
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpec& spec) : m_Spec(spec)
	{
		Invalidate();
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_FrameBufferID);
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		if (!frame_buffer_created)
		{
			glCreateFramebuffers(1, &m_FrameBufferID);
			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
			
			glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachmentT);
			glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentT);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Spec.width, m_Spec.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			glTextureStorage2D(m_ColorAttachmentT, 1, GL_RGB8, m_Spec.width, m_Spec.height);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentT, 0);
			
			glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachmentT);
			glBindTexture(GL_TEXTURE_2D, m_DepthAttachmentT);
			glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Spec.width, m_Spec.height);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_Spec.width, m_Spec.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachmentT, 0);
			frame_buffer_created = 1;
		}
		else {
			glTextureStorage2D(m_ColorAttachmentT, 1, GL_RGB8, m_Spec.width, m_Spec.height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentT, 0);
		}


		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			XEN_ENGINE_LOG_ERROR("FrameBuffer is imcomplete!");
			TRIGGER_BREAKPOINT;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Spec.width = width;
		m_Spec.height = height;
		OpenGLFrameBuffer::Invalidate();
	}

	void OpenGLFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
	}
	void OpenGLFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	uint32_t OpenGLFrameBuffer::GetColorAttachmentRendererID() const
	{
		return m_ColorAttachmentT;
	}
}