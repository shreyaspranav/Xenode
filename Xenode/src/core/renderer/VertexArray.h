#pragma once

#include <Core.h>
#include "Buffer.h"

namespace Xen {
	class XEN_API VertexArray
	{
	public:
		virtual void SetVertexBuffer(const Ref<FloatBuffer>& vertexBuffer) = 0;
		virtual void SetElementBuffer(const Ref<ElementBuffer>& elementBuffer) = 0;

		virtual void Load(bool hasIndexBuffer) = 0;
	
		virtual inline void Bind() const = 0;
		virtual inline void Unbind() const = 0;

		virtual inline Ref<FloatBuffer> GetVertexBuffer() = 0;
		virtual inline Ref<ElementBuffer> GetElementBuffer() = 0;

		virtual inline uint8_t GetStrideCount() = 0;

		static Ref<VertexArray> CreateVertexArray();
	};
}
