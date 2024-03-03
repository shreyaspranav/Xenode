#pragma once

#include <core/renderer/Buffer.h>

namespace Xen {
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		friend class OpenGLTransformFeedback;

	public:
		OpenGLVertexBuffer(Size size, const VertexBufferLayout& layout);
		virtual ~OpenGLVertexBuffer();

		void Put(const void* data, Size size) override;
		void Put(Size offsetSize, const void* data, Size size) override;

		void SetElementBuffer(const Ref<ElementBuffer>& elementBuffer) override;

		inline const VertexBufferLayout& GetBufferLayout() const override { return m_BufferLayout; }

		inline Ref<ElementBuffer> GetElementBuffer() const override { return m_ElementBuffer; }

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

		Ref<ElementBuffer> m_ElementBuffer = nullptr;

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
		uint32_t m_BufferID, m_Count, m_ActiveCount;
		Size m_Size;
		ElementBufferDataType m_ElementDataType;
	};

	class OpenGLTransformFeedback : public TransformFeedback
	{
	public:
		friend class OpenGLShader;
	public:
		OpenGLTransformFeedback(std::vector<std::string> outAttributes, TransformFeedbackPrimitive primitive);
		virtual ~OpenGLTransformFeedback();

		void BeginFeedback() override;
		void EndFeedback() override;

		void Bind() override;
		void Unbind() override;

		void SetFeedbackBuffer(const Ref<VertexBuffer> vertexBuffer) override;
		void RegisterTransformFeedback(const Shader* shader) override;

	private:

	private:
		Ref<VertexBuffer> m_VertexBuffer;
		TransformFeedbackPrimitive m_TFeedbackPrimitive;

		uint32_t m_BufferID;
		Size m_Size;
		std::vector<std::string> m_Attribs;
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
