#include "pch"
#include "OpenGLFrameBuffer.h"

#include <glad/gl.h>

#include <core/app/Log.h>
#include "OpenGLContext.h"

#include <core/app/Profiler.h>

namespace Xen {
	OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferSpec& spec) : m_Spec(spec)
	{
		m_DepthAttachmentT = 0;
		OpenGLFrameBuffer::Invalidate();	
	}
	OpenGLFrameBuffer::~OpenGLFrameBuffer()
	{
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());

		if(m_DepthAttachmentT)
			glDeleteTextures(1, &m_DepthAttachmentT);

		glDeleteFramebuffers(1, &m_FrameBufferID);
	}

	void OpenGLFrameBuffer::Invalidate()
	{
		XEN_PROFILE_FN();

		OpenGLFrameBuffer::CreateTextures();

		if (!m_FrameBufferCreated) {
			glCreateFramebuffers(1, &m_FrameBufferID);
			m_FrameBufferCreated = true;
		}
		else {
			// Delete Textures? But seems to work fine without it.
		}

		for (int i = 0; i < m_ColorAttachments.size(); i++)
			glNamedFramebufferTexture(m_FrameBufferID, GL_COLOR_ATTACHMENT0 + i, m_ColorAttachments[i], 0);
		if (m_DepthAttachmentT)
			glNamedFramebufferTexture(m_FrameBufferID, GL_DEPTH_STENCIL_ATTACHMENT, m_DepthAttachmentT, 0);


		if (m_ColorAttachments.size() >= 1)
		{
			GLenum draw_buffers[4] = {
				GL_COLOR_ATTACHMENT0,
				GL_COLOR_ATTACHMENT1,
				GL_COLOR_ATTACHMENT2,
				GL_COLOR_ATTACHMENT3,
			};
			glNamedFramebufferDrawBuffers(m_FrameBufferID, m_ColorAttachments.size(), draw_buffers);
		}
		else
			glNamedFramebufferDrawBuffer(m_FrameBufferID, GL_NONE);

		if (glCheckNamedFramebufferStatus(m_FrameBufferID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			XEN_ENGINE_LOG_ERROR_SEVERE("Framebuffer Not Complete!");
			TRIGGER_BREAKPOINT;
		}
	}

	void SetupTexture(uint32_t texture_id, uint32_t width, uint32_t height, GLenum internal_format, GLenum format, FrameBufferFiltering filtering, uint32_t samples)
	{
		if (samples > 1)
			glTextureStorage2DMultisample(texture_id, samples, internal_format, width, height, GL_FALSE);
		else
		{
			glTextureStorage2D(texture_id, 1, internal_format, width, height);

			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, filtering == FrameBufferFiltering::Linear ? GL_LINEAR : GL_NEAREST);
			glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, filtering == FrameBufferFiltering::Linear ? GL_LINEAR : GL_NEAREST);
		}
	}

	void OpenGLFrameBuffer::CreateTextures()
	{
		uint8_t color_attachment_count = 0, depth_attachment_count = 0;
		uint8_t depth_att_index = 0;

		for (int i = 0; i < m_Spec.attachments.size(); i++) 
		{
			if (m_Spec.attachments[i].format <= FrameBufferTextureFormat::ColorAttachment_Last)
				color_attachment_count++;
			else {
				depth_att_index = i;
				depth_attachment_count++;
			}
		}

		// Make sure that the depth buffer count is either 1 or 0
		if (depth_attachment_count > 1) 
		{
			XEN_ENGINE_LOG_ERROR("Multiple depth attachements not supported by OpenGL!");
			TRIGGER_BREAKPOINT;
		}

		if (color_attachment_count > 4)
		{
			XEN_ENGINE_LOG_ERROR("Color attachments exceeded 4");
			TRIGGER_BREAKPOINT;
		}

		// Check to See if the textures are multisampled from the Framebuffer Specification
		m_Multisampled = m_Spec.samples > 1;
		
		m_ColorAttachments.resize(color_attachment_count);

		// Create the textures:
		glCreateTextures(m_Multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 
			color_attachment_count, 
			m_ColorAttachments.data());

		if(depth_attachment_count)
			glCreateTextures(m_Multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 
				depth_attachment_count, 
				&m_DepthAttachmentT);

		uint8_t color_att_index = 0;

		for (int i = 0; i < m_Spec.attachments.size(); i++)
		{
			switch (m_Spec.attachments[i].format)
			{
			case FrameBufferTextureFormat::None:
				XEN_ENGINE_LOG_ERROR("Framebuffer Format None is not supported!");
				TRIGGER_BREAKPOINT;
				break;
			case FrameBufferTextureFormat::RI:
				SetupTexture(m_ColorAttachments[color_att_index],
					m_Spec.width, m_Spec.height,
					GL_R32I,
					GL_RED_INTEGER,
					m_Spec.attachments[i].filtering,
					m_Spec.samples);
				break;
			case FrameBufferTextureFormat::RGB8:
				SetupTexture(m_ColorAttachments[color_att_index],
					m_Spec.width, m_Spec.height,
					GL_RGB8,
					GL_RGBA,
					m_Spec.attachments[i].filtering,
					m_Spec.samples);
				break;
			case FrameBufferTextureFormat::RGB16F:
				SetupTexture(m_ColorAttachments[color_att_index],
					m_Spec.width, m_Spec.height,
					GL_RGB16F,
					GL_RGBA,
					m_Spec.attachments[i].filtering,
					m_Spec.samples);
				break;
			case FrameBufferTextureFormat::RGB32F:
				SetupTexture(m_ColorAttachments[color_att_index],
					m_Spec.width, m_Spec.height,
					GL_RGB32F,
					GL_RGBA,
					m_Spec.attachments[i].filtering,
					m_Spec.samples);
				break;
			default:
				break;
			}
			color_att_index++;
		}
		
		if (depth_attachment_count) {
			switch (m_Spec.attachments[depth_att_index].format)
			{
			case FrameBufferTextureFormat::Depth24_Stencil8:
				SetupTexture(m_DepthAttachmentT,
					m_Spec.width, m_Spec.height,
					GL_DEPTH24_STENCIL8,
					GL_DEPTH24_STENCIL8,
					m_Spec.attachments[depth_att_index].filtering,
					m_Spec.samples);
				break;
			case FrameBufferTextureFormat::Depth32F_Stencil8:
				SetupTexture(m_DepthAttachmentT,
					m_Spec.width, m_Spec.height,
					GL_DEPTH32F_STENCIL8,
					GL_DEPTH32F_STENCIL8,
					m_Spec.attachments[depth_att_index].filtering,
					m_Spec.samples);
				break;
			default:
				break;
			}
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
		XEN_PROFILE_FN();
		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
	}

	void OpenGLFrameBuffer::Unbind()
	{
		XEN_PROFILE_FN();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFrameBuffer::ClearAttachments()
	{

		uint8_t color_att_index = 0;
		for (int i = 0; i < m_Spec.attachments.size(); i++)
		{
			float clearColor[4] = {
				m_Spec.attachments[i].clearColor.r,
				m_Spec.attachments[i].clearColor.g,
				m_Spec.attachments[i].clearColor.b,
				m_Spec.attachments[i].clearColor.a,
			};

			int r_color = (int)m_Spec.attachments[i].clearColor.r;

			switch (m_Spec.attachments[i].format)
			{
			case FrameBufferTextureFormat::None:
				XEN_ENGINE_LOG_ERROR("Framebuffer Format None is not supported!");
				TRIGGER_BREAKPOINT;
				break;
			case FrameBufferTextureFormat::RI:
				glClearTexImage(m_ColorAttachments[color_att_index], 0, GL_RED_INTEGER, GL_INT, &r_color);
				break;
			case FrameBufferTextureFormat::RGB8:
				glClearTexImage(m_ColorAttachments[color_att_index], 0, GL_RGBA, GL_FLOAT, clearColor);
				break;
			case FrameBufferTextureFormat::RGB16F:
				break;
			case FrameBufferTextureFormat::RGB32F:
				break;
			default:
				break;
			}
			color_att_index++;
		}
	}

	uint32_t OpenGLFrameBuffer::GetColorAttachmentRendererID(uint32_t index) const
	{
		if (index > m_ColorAttachments.size() - 1)
		{
			XEN_ENGINE_LOG_ERROR("\'index\' out of range");
			TRIGGER_BREAKPOINT;
		}
		return m_ColorAttachments[index];
	}
	int32_t OpenGLFrameBuffer::ReadIntPixel(uint32_t index, int32_t x, int32_t y)
	{
		glNamedFramebufferReadBuffer(m_FrameBufferID, GL_COLOR_ATTACHMENT0 + index);
		int data;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &data);
		return data;
	}
}