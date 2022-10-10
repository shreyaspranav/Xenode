#pragma once

#include <core/renderer/Buffer.h>

namespace Xen {
	class OpenGLFloatBuffer : public FloatBuffer
	{
	public:
		OpenGLFloatBuffer(uint32_t count);
		virtual ~OpenGLFloatBuffer();

		void Put(float* data, uint32_t count) override;
		void Put(uint32_t offsetCount, float* data, uint32_t count) override;

		inline void SetBufferLayout(const BufferLayout& layout) override { m_BufferLayout = layout; }
		inline const BufferLayout& GetBufferLayout() const override { return m_BufferLayout; }

		inline void Bind() const override;
		inline void Unbind() const override;

		inline uint32_t GetCount() const override;
		inline uint32_t GetSize() const override;

	private:
		uint32_t m_BufferID, m_Size, m_Count;
		BufferLayout m_BufferLayout;
	};

	class OpenGLElementBuffer : public ElementBuffer
	{
	public:
		OpenGLElementBuffer(uint32_t count);
		virtual ~OpenGLElementBuffer();

		void Put(uint32_t* data, uint32_t count) override;
		void Put(uint32_t offsetCount, uint32_t* data, uint32_t count) override;

		inline void Bind() const override;
		inline void Unbind() const override;

		inline uint32_t GetCount() const override;
		inline uint32_t GetActiveCount() const override;
		inline uint32_t GetSize() const override;

	private:
		uint32_t m_BufferID, m_Size, m_Count, m_ActiveCount;

	};
}
