#pragma once

#include <core/renderer/FrameBuffer.h>

namespace Xen {
	class XEN_API OpenGLFrameBuffer : public FrameBuffer
	{
	public:
		OpenGLFrameBuffer() {}
		OpenGLFrameBuffer(const FrameBufferSpec& spec);
		virtual ~OpenGLFrameBuffer();

		void Invalidate();
		void Resize(uint32_t width, uint32_t height) override;

		void Bind() override;
		void Unbind() override;

		uint32_t GetColorAttachmentRendererID() const override;

		const FrameBufferSpec& GetFrameBufferSpecification() const override { return m_Spec; }

	private:
		uint32_t m_FrameBufferID;
		uint32_t m_ColorAttachmentT, m_DepthAttachmentT;
		bool frame_buffer_created = 0;

		FrameBufferSpec m_Spec;
	};
}
