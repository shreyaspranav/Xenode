#pragma once

#include <Core.h>

namespace Xen {

	enum class VertexBufferDataType
	{
		// GLSL: float, vec2, vec3, vec4:
		Float, Float2, Float3, Float4,

		// GLSL: int, ivec2, ivec3, ivec4:
		Int, Int2, Int3, Int4,

		// GLSL: uint, uvec2, uvec3, uvec4:
		UnsignedInt, UnsignedInt2, UnsignedInt3, UnsignedInt4,

		// GLSL: mat3, mat4: 
		Mat3, Mat4
	};

	enum class ElementBufferDataType { Unsigned16Bit, Unsigned32Bit };

	enum class TransformFeedbackPrimitive { Points, Lines, Triangles };

	struct VertexBufferElement
	{
		std::string name;
		VertexBufferDataType type;
		uint8_t shader_location;

		VertexBufferElement(const std::string& name, VertexBufferDataType type, uint8_t shader_location)
			: name(name), type(type), shader_location(shader_location)
		{}
	};

	class VertexBufferLayout
	{
	public:
		VertexBufferLayout() = default;
		VertexBufferLayout(std::initializer_list<VertexBufferElement> elements)
			: m_BufferElements(elements) {}

		inline void AddBufferElement(const VertexBufferElement& element) { m_BufferElements.push_back(element); }

		inline const std::vector<VertexBufferElement>& GetBufferElements() const { return m_BufferElements; }

		inline std::vector<VertexBufferElement>::iterator StartIterator() { return m_BufferElements.begin(); }
		inline std::vector<VertexBufferElement>::iterator EndIterator() { return m_BufferElements.end(); }
		inline std::vector<VertexBufferElement>::const_iterator StartIterator() const { return m_BufferElements.begin(); }
		inline std::vector<VertexBufferElement>::const_iterator EndIterator() const { return m_BufferElements.end(); }

		inline std::vector<VertexBufferElement>::reverse_iterator StartIteratorReverse() { return m_BufferElements.rbegin(); }
		inline std::vector<VertexBufferElement>::reverse_iterator EndIteratorReverse() { return m_BufferElements.rend(); }
		inline std::vector<VertexBufferElement>::const_reverse_iterator StartIteratorReverse() const { return m_BufferElements.rbegin(); }
		inline std::vector<VertexBufferElement>::const_reverse_iterator EndIteratorReverse() const { return m_BufferElements.rend(); }
	private:
		std::vector<VertexBufferElement> m_BufferElements;
	};

	class XEN_API ElementBuffer
	{
	public:
		virtual void Put(const void* data, Size size) = 0;
		virtual void Put(Size offsetSize, const void* data, Size size) = 0;


		virtual inline void Bind() const = 0;
		virtual inline void Unbind() const = 0;

		virtual inline uint32_t GetCount() const = 0;
		virtual inline uint32_t GetActiveCount() const = 0;
		virtual inline uint32_t GetSize() const = 0;
		virtual inline ElementBufferDataType GetElementBufferDataType() const = 0;

		static Ref<ElementBuffer> CreateElementBuffer(Size size, ElementBufferDataType dataType);
	};

	class XEN_API VertexBuffer
	{
	public:
		virtual void Put(const void* data, Size size) = 0;
		virtual void Put(Size offsetSize, const void* data, Size size) = 0;

		virtual void SetElementBuffer(const Ref<ElementBuffer>& elementBuffer) = 0;

		virtual const VertexBufferLayout& GetBufferLayout() const = 0;

		virtual inline void Bind() const = 0;
		virtual inline void Unbind() const = 0;

		virtual inline uint32_t GetCount() const = 0;
		virtual inline uint32_t GetSize() const = 0;
		virtual inline Ref<ElementBuffer> GetElementBuffer() const = 0;

		virtual inline bool HasElementBuffer() const = 0;

		static Ref<VertexBuffer> CreateVertexBuffer(Size size, const VertexBufferLayout& layout);
	};


	class Shader;

	class XEN_API TransformFeedback
	{
	public:
		friend class Shader;

		virtual void SetFeedbackBuffer(const Ref<VertexBuffer> buffer) = 0;
		virtual void BeginFeedback() = 0;
		virtual void EndFeedback() = 0;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void RegisterTransformFeedback(const Shader* shader) = 0;
		static Ref<TransformFeedback> CreateTransformFeedback(std::vector<std::string> outAttributes, TransformFeedbackPrimitive primitive);
	private:
	};


	// In both of these buffers, the VertexBufferLayout is just not used.
	// That is just for future.

	// IMPORTANT!: For Uniform Buffers use std140 layout and for the storage buffers use std430 layout.
	class XEN_API UniformBuffer
	{
	public:
		virtual void Put(Size offset, const void* data, Size size) = 0;

		static Ref<UniformBuffer> CreateUniformBuffer(Size size, const VertexBufferLayout& layout, uint8_t bindingIndex);
	};

	class XEN_API StorageBuffer
	{
	public:
		virtual void Put(Size offset, const void* data, Size size) = 0;

		static Ref<StorageBuffer> CreateStorageBuffer(Size size, const VertexBufferLayout& layout, uint8_t bindingIndex);
	};
}

