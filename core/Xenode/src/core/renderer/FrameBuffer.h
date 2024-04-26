#pragma once

#include <Core.h>
#include "Structs.h"

#include "Texture.h"

namespace Xen {

	enum class FrameBufferTextureFormat
	{
		None = 0,

		// Color Attachments:
		RI			= 1,
		RGB8		= 2,
		RGB16F		= 3,
		RGB32F		= 4,
		R11G11B10F  = 5,

		ColorAttachment_Last = R11G11B10F,

		// Depth Attachements:
		Depth24_Stencil8   = 7,
		Depth32F_Stencil8  = 8,

		DepthAttachment_Last = Depth24_Stencil8
	};

	enum class FrameBufferFiltering { Linear, Nearest };

	struct FrameBufferAttachmentSpec
	{
		FrameBufferAttachmentSpec() = default;
		FrameBufferAttachmentSpec(FrameBufferTextureFormat tex_format)
			:format(tex_format) {}

		FrameBufferTextureFormat format = FrameBufferTextureFormat::None;
		FrameBufferFiltering filtering = FrameBufferFiltering::Linear;
		Color clearColor;

		uint8_t mipmaps = 0;
		bool resizable = false;
	};

	struct FrameBufferSpec
	{
		uint32_t width, height;
		std::vector<FrameBufferAttachmentSpec> attachments;
		uint8_t samples = 1;

		bool target_swap_chain = false;
	};

	class XEN_API FrameBuffer
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual const FrameBufferSpec& GetFrameBufferSpecification() const = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void ClearAttachments() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual int32_t ReadIntPixel(uint32_t index, int32_t x, int32_t y) = 0;
		virtual uint32_t GetColorAttachmentRendererID(uint8_t index) const = 0;

		virtual void SetClearColor(uint32_t index, const Color& color) = 0;

		static Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferSpec& spec);
	};
}
