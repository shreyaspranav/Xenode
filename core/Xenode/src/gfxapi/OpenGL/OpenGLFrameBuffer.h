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
		void ClearAttachments() override;

		uint32_t GetColorAttachmentRendererID(uint8_t index) const override;
		int32_t ReadIntPixel(uint32_t index, int32_t x, int32_t y) override;

		void SetClearColor(uint32_t index, const Color& color) override;
		void SetActiveColorAttachments(std::initializer_list<uint32_t> indexList) override;

		const FrameBufferSpec& GetFrameBufferSpecification() const override { return m_Spec; }

	private:
		void CreateTextures();

	private:
		uint32_t m_FrameBufferID;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachmentT;

		bool m_FrameBufferCreated = false;
		bool m_Multisampled = false;

		FrameBufferSpec m_Spec;
	};
}
