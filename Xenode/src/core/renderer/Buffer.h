#pragma once

#include <Core.h>

namespace Xen {

	enum class BufferDataType { Float, Int, UnsignedInt };

	struct BufferElement
	{
		std::string name;
		uint8_t shader_location;
		uint8_t count_per_vertex;
		uint8_t offset_count;
		BufferDataType type;

		bool normalized;

		BufferElement(const std::string& name, uint8_t shader_location, uint8_t count_per_vertex, uint8_t offset_count, BufferDataType type, bool normalized)
			: name(name), shader_location(shader_location), count_per_vertex(count_per_vertex), offset_count(offset_count), type(type), normalized(normalized)
		{}
	};

	class BufferLayout
	{
	public:
		inline void AddBufferElement(const BufferElement& element) { m_BufferElements.push_back(element); }

		inline const std::vector<BufferElement>& GetBufferElements() { return m_BufferElements; }
	private:
		std::vector<BufferElement> m_BufferElements;
	};

	class XEN_API FloatBuffer
	{
	public:
		virtual void Put(float* data, uint32_t count) = 0;
		virtual void Put(uint32_t offsetCount, float* data, uint32_t count) = 0;

		virtual inline void SetBufferLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetBufferLayout() const = 0;

		virtual inline void Bind() const = 0;
		virtual inline void Unbind() const = 0;

		virtual inline uint32_t GetCount() const = 0;
		virtual inline uint32_t GetSize() const = 0;

		static Ref<FloatBuffer> CreateFloatBuffer(uint32_t count);
	};

	class XEN_API ElementBuffer
	{
	public:
		virtual void Put(uint32_t* data, uint32_t count) = 0;
		virtual void Put(uint32_t offsetCount, uint32_t* data, uint32_t count) = 0;

		virtual inline void Bind() const = 0;
		virtual inline void Unbind() const = 0;

		virtual inline uint32_t GetCount() const = 0;
		virtual inline uint32_t GetActiveCount() const = 0;
		virtual inline uint32_t GetSize() const = 0;

		static Ref<ElementBuffer> CreateElementBuffer(uint32_t count);
	};
}

