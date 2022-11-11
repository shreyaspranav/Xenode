#pragma once

#include <Core.h>

namespace Xen {

	struct FrameBufferSpec
	{
		uint32_t width, height;
		uint32_t samples = 1;

		bool target_swap_chain = false;
	};

	class XEN_API FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual const FrameBufferSpec& GetFrameBufferSpecification() const = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual uint32_t GetColorAttachmentRendererID() const = 0;

		static Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferSpec& spec);
	};
}
