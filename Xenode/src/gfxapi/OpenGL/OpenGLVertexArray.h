#pragma once

#include <Core.h>
#include <core/renderer/VertexArray.h>

namespace Xen {
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();

		void SetVertexBuffer(const Ref<FloatBuffer>& vertexBuffer) override;
		void SetElementBuffer(const Ref<ElementBuffer>& elementBuffer) override;

		void Load(bool hasIndexBuffer) override;

		inline void Bind() const override;
		inline void Unbind() const override;

		inline Ref<FloatBuffer> GetVertexBuffer() override { return m_VertexBuffer; }
		inline Ref<ElementBuffer> GetElementBuffer() override { return m_ElementBuffer; }

		inline uint8_t GetStrideCount() override { return m_StrideCount; }

	private:
		Ref<FloatBuffer> m_VertexBuffer;
		Ref<ElementBuffer> m_ElementBuffer;

		uint32_t m_VertexArrayID;
		uint8_t m_StrideCount;
	};
}
