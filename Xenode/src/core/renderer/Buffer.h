#pragma once

#include <Core.h>

namespace Xen {
	class XEN_API FloatBuffer
	{
	public:
		virtual void Put(float* data, uint32_t count) = 0;
		virtual void Put(uint32_t offsetCount, float* data, uint32_t count) = 0;

		virtual inline void Bind() = 0;
		virtual inline void Unbind() = 0;

		virtual inline uint32_t GetCount() = 0;
		virtual inline uint32_t GetSize() = 0;

		static Ref<FloatBuffer> CreateFloatBuffer(uint32_t count);
	};

	class XEN_API ElementBuffer
	{
	public:
		virtual void Put(uint32_t* data, uint32_t count) = 0;
		virtual void Put(uint32_t offsetCount, uint32_t* data, uint32_t count) = 0;

		virtual inline void Bind() = 0;
		virtual inline void Unbind() = 0;

		virtual inline uint32_t GetCount() = 0;
		virtual inline uint32_t GetSize() = 0;

		static Ref<ElementBuffer> CreateElementBuffer(uint32_t count);
	};
}

