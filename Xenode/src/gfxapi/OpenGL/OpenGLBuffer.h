#pragma once

#include <core/renderer/Buffer.h>

namespace Xen {
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(Size size, const VertexBufferLayout& layout);
		virtual ~OpenGLVertexBuffer();

		void Put(const void* data, Size size) override;
		void Put(Size offsetSize, const void* data, Size size) override;

		void SetElementBuffer(const Ref<ElementBuffer>& elementBuffer) override;

		inline const VertexBufferLayout& GetBufferLayout() const override { return m_BufferLayout; }

		inline void Bind() const override;
		inline void Unbind() const override;

		inline uint32_t GetCount() const override;
		inline uint32_t GetSize() const override;

		inline bool HasElementBuffer() const override;

	private:
		uint32_t m_VertexBufferID, m_VertexArrayID;
		uint32_t m_Count;
		Size m_Size;
		VertexBufferLayout m_BufferLayout;

		bool m_HasElementBuffer;
	};

	class OpenGLElementBuffer : public ElementBuffer
	{
	public:
		friend class OpenGLVertexBuffer;
	public:
		OpenGLElementBuffer(Size size, ElementBufferDataType type);
		virtual ~OpenGLElementBuffer();

		void Put(const void* data, Size size) override;
		void Put(Size offsetSize, const void* data, Size size) override;

		inline void Bind() const override;
		inline void Unbind() const override;

		inline uint32_t GetCount() const override;
		inline uint32_t GetActiveCount() const override;
		inline uint32_t GetSize() const override;
		inline ElementBufferDataType GetElementBufferDataType() const override;

	private:
		uint32_t m_BufferID, m_Size, m_Count, m_ActiveCount;
		ElementBufferDataType m_ElementDataType;
	};

	class OpenGLUniformBuffer : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(Size size, const VertexBufferLayout& layout, uint8_t bindingIndex);
		virtual ~OpenGLUniformBuffer();

		void Put(Size offset, const void* data, Size size) override;
	private:
		uint32_t m_BufferID;

		Size m_Size;
		VertexBufferLayout m_Layout;
	};

	class OpenGLStorageBuffer : public StorageBuffer
	{
	public:
		OpenGLStorageBuffer(Size size, const VertexBufferLayout& layout, uint8_t bindingIndex);
		virtual ~OpenGLStorageBuffer();

		void Put(Size offset, const void* data, Size size) override;
	private:
		uint32_t m_BufferID;

		Size m_Size;
		VertexBufferLayout m_Layout;
	};
}
