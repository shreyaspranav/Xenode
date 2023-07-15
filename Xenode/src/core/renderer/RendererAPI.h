#pragma once

#include "Structs.h"
#include "VertexArray.h"

namespace Xen {
	enum BlendFactor
	{
		Zero, One,
		DstColor, OneMinusDstColor,
		DstAlpha, OneMinusDstAlpha,
		SrcColor, OneMinusSrcColor,
		SrcAlpha, OneMinusSrcAlpha
	};

	enum BlendOperation { Add, Subtract, ReverseSubtract };

	struct BlendMode
	{
		BlendFactor srcFactor, dstFactor;
		BlendOperation blendOperation;

		BlendMode(const BlendMode& mode) = default;
		BlendMode(BlendFactor src, BlendFactor dst, BlendOperation operation)
			: srcFactor(src), dstFactor(dst), blendOperation(operation) {}
	};

	class RendererAPI
	{
	public:
		virtual void Clear() = 0;
		virtual void SetClearColor(const Color& color) = 0;
		virtual void OnWindowResize(uint32_t width, uint32_t height) = 0;
		
		virtual void SetBlendMode(BlendMode colorBlendMode, BlendMode alphaBlendMode) = 0;

		//Draw Commands
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indices) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t indices) = 0;
		virtual void DrawTriangles(const Ref<VertexArray>& vertexArray, uint32_t indices) = 0;

		virtual void SetLineWidth(float width) = 0;
	};
}
