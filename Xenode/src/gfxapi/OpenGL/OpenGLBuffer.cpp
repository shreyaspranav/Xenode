#include "pch"
#include "OpenGLBuffer.h"
#include "core/app/Log.h"

#include <glad/gl.h>

#include <core/app/Profiler.h>

namespace Xen {

	static int8_t CalculateCount(const VertexBufferDataType& elementDataType)
	{
		switch (elementDataType)
		{
		case VertexBufferDataType::Float:  return 1;
		case VertexBufferDataType::Float2: return 2;
		case VertexBufferDataType::Float3: return 3;
		case VertexBufferDataType::Float4: return 4;

		case VertexBufferDataType::Int:  return 1;
		case VertexBufferDataType::Int2: return 2;
		case VertexBufferDataType::Int3: return 3;
		case VertexBufferDataType::Int4: return 4;

		case VertexBufferDataType::UnsignedInt:  return 1;
		case VertexBufferDataType::UnsignedInt2: return 2;
		case VertexBufferDataType::UnsignedInt3: return 3;
		case VertexBufferDataType::UnsignedInt4: return 4;

		case VertexBufferDataType::Mat3: return 3 * 3;
		case VertexBufferDataType::Mat4: return 4 * 4;
		}
	}

	static Size CalculateSize(const VertexBufferDataType& elementDataType)
	{
		Size sizeOfFloat = sizeof(float);
		Size sizeOfInt = sizeof(int);

		switch (elementDataType)
		{
		case VertexBufferDataType::Float:  return 1 * sizeOfFloat;
		case VertexBufferDataType::Float2: return 2 * sizeOfFloat;
		case VertexBufferDataType::Float3: return 3 * sizeOfFloat;
		case VertexBufferDataType::Float4: return 4 * sizeOfFloat;

		case VertexBufferDataType::Int:  return 1 * sizeOfInt;
		case VertexBufferDataType::Int2: return 2 * sizeOfInt;
		case VertexBufferDataType::Int3: return 3 * sizeOfInt;
		case VertexBufferDataType::Int4: return 4 * sizeOfInt;

		case VertexBufferDataType::UnsignedInt:  return 1 * sizeOfInt;
		case VertexBufferDataType::UnsignedInt2: return 2 * sizeOfInt;
		case VertexBufferDataType::UnsignedInt3: return 3 * sizeOfInt;
		case VertexBufferDataType::UnsignedInt4: return 4 * sizeOfInt;

		case VertexBufferDataType::Mat3: return 3 * 3 * sizeOfFloat;
		case VertexBufferDataType::Mat4: return 4 * 4 * sizeOfFloat;
		}
	}

	//-------OpenGLVertexBuffer---------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------
	OpenGLVertexBuffer::OpenGLVertexBuffer(Size size, const VertexBufferLayout& layout) 
		: m_Size(size), m_BufferLayout(layout), m_HasElementBuffer(false)
	{
		glCreateBuffers(1, &m_VertexBufferID);
		glNamedBufferStorage(m_VertexBufferID, m_Size, nullptr, GL_DYNAMIC_STORAGE_BIT);

		// Even though we are using DSA functions, A bug in intel drivers will cause the shader to not read integer attributes for some reason.
		// Binding to the VAO fixed it.
		glCreateVertexArrays(1, &m_VertexArrayID);

		Size stride = 0;
		for (auto it = layout.StartIterator(); it != layout.EndIterator(); it++)
			stride += CalculateSize((*it).type);

		// m_Count -> No of vertices.
		m_Count = size / stride;

		glBindVertexArray(m_VertexArrayID);

		glVertexArrayVertexBuffer(m_VertexArrayID, 0, m_VertexBufferID, 0, stride);

		uint8_t iterationCount = 0;
		Size offset = 0;
		for (auto it = layout.StartIterator(); it != layout.EndIterator(); it++)
		{
			glEnableVertexArrayAttrib(m_VertexArrayID, (*it).shader_location);

			switch ((*it).type)
			{
			case VertexBufferDataType::Float: 
			case VertexBufferDataType::Float2:
			case VertexBufferDataType::Float3:
			case VertexBufferDataType::Float4:
				glVertexArrayAttribFormat(m_VertexArrayID, (*it).shader_location,
					CalculateCount((*it).type), GL_FLOAT, GL_FALSE, offset);
				offset += CalculateSize((*it).type);
				break;

			case VertexBufferDataType::Int:
			case VertexBufferDataType::Int2:
			case VertexBufferDataType::Int3:
			case VertexBufferDataType::Int4:
				// For Integer formats should use a different funtion. Facepalm! wasted 1 hour on this:
				glVertexArrayAttribIFormat(m_VertexArrayID, (*it).shader_location,
					CalculateCount((*it).type), GL_INT, offset);
				offset += CalculateSize((*it).type);
				break;

			case VertexBufferDataType::UnsignedInt:
			case VertexBufferDataType::UnsignedInt2:
			case VertexBufferDataType::UnsignedInt3:
			case VertexBufferDataType::UnsignedInt4:
				glVertexArrayAttribIFormat(m_VertexArrayID, (*it).shader_location,
					CalculateCount((*it).type), GL_UNSIGNED_INT, offset);
				offset += CalculateSize((*it).type);
				break;
			}

			//glVertexArrayVertexBuffer(m_VertexArrayID, 0, m_VertexBufferID, 0, stride);
			glVertexArrayAttribBinding(m_VertexArrayID, (*it).shader_location, 0);

			iterationCount++;
		}
	}
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_VertexBufferID);
	}
	void OpenGLVertexBuffer::Put(const void* data, Size size)
	{
		OpenGLVertexBuffer::Put(0, data, size);
	}
	void OpenGLVertexBuffer::Put(Size offsetSize, const void* data, Size size)
	{
		XEN_PROFILE_FN();

		if (size > m_Size - offsetSize)
		{
			XEN_ENGINE_LOG_ERROR("VertexBuffer overflow!");
			TRIGGER_BREAKPOINT;
		}

		glNamedBufferSubData(m_VertexBufferID, offsetSize, size, data);
	}

	void OpenGLVertexBuffer::SetElementBuffer(const Ref<ElementBuffer>& elementBuffer)
	{
		Ref<OpenGLElementBuffer> openglElementBuffer = std::dynamic_pointer_cast<OpenGLElementBuffer>(elementBuffer);
		glVertexArrayElementBuffer(m_VertexArrayID, openglElementBuffer->m_BufferID);

		m_HasElementBuffer = true;
	}
	inline bool OpenGLVertexBuffer::HasElementBuffer() const
	{
		return m_HasElementBuffer;
	}

	inline void OpenGLVertexBuffer::Bind() const				{ XEN_PROFILE_FN(); glBindVertexArray(m_VertexArrayID); }
	inline void OpenGLVertexBuffer::Unbind() const				{ glBindVertexArray(0); }
	inline uint32_t OpenGLVertexBuffer::GetCount() const		{ return m_Count; }
	inline uint32_t OpenGLVertexBuffer::GetSize() const			{ return m_Size; }

	//-------OpenGLElementBuffer---------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	OpenGLElementBuffer::OpenGLElementBuffer(Size size, ElementBufferDataType type) 
		: m_Size(size), m_ElementDataType(type)
	{
		glCreateBuffers(1, &m_BufferID);
		glNamedBufferStorage(m_BufferID, m_Size, nullptr, GL_DYNAMIC_STORAGE_BIT);

		m_Count = size / (type == ElementBufferDataType::Unsigned32Bit ? sizeof(int) : sizeof(short));

		m_ActiveCount = 0;
	}
	OpenGLElementBuffer::~OpenGLElementBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}
	void OpenGLElementBuffer::Put(const void* data, Size size)
	{
		OpenGLElementBuffer::Put(0, data, size);
		//m_ActiveCount += size;
	}
	void OpenGLElementBuffer::Put(Size offsetSize, const void* data, Size size)
	{
		XEN_PROFILE_FN();

		if (size > m_Size - offsetSize)
		{
			XEN_ENGINE_LOG_ERROR("VertexBuffer overflow!");
			TRIGGER_BREAKPOINT;
		}

		glNamedBufferSubData(m_BufferID, offsetSize, size, data);
	}

	inline ElementBufferDataType OpenGLElementBuffer::GetElementBufferDataType() const
	{
		return m_ElementDataType;
	}

	inline void OpenGLElementBuffer::Bind() const					{ XEN_PROFILE_FN();  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID); }
	inline void OpenGLElementBuffer::Unbind() const					{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	inline uint32_t OpenGLElementBuffer::GetCount() const			{ return m_Count; }
	inline uint32_t OpenGLElementBuffer::GetActiveCount() const		{ return m_ActiveCount; }

	inline uint32_t OpenGLElementBuffer::GetSize() const	{ return m_Size; }
}