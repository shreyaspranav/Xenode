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

		inline void Bind() override;
		inline void Unbind() override;

		inline uint32_t GetCount() override;
		inline uint32_t GetSize() override;

	private:
		uint32_t m_BufferID, m_Size, m_Count;
	};

	class OpenGLElementBuffer : public ElementBuffer
	{
	public:
		OpenGLElementBuffer(uint32_t count);
		virtual ~OpenGLElementBuffer();

		void Put(uint32_t* data, uint32_t count) override;
		void Put(uint32_t offsetCount, uint32_t* data, uint32_t count) override;

		inline void Bind() override;
		inline void Unbind() override;

		inline uint32_t GetCount() override;
		inline uint32_t GetSize() override;

	private:
		uint32_t m_BufferID, m_Size, m_Count;

	};
}
