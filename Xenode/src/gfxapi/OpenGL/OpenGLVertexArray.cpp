#include "pch"
#include "OpenGLVertexArray.h"

#include <glad/gl.h>

namespace Xen {
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_VertexArrayID);
	}
	void OpenGLVertexArray::SetVertexBuffer(const Ref<FloatBuffer>& vertexBuffer)
	{
		m_VertexBuffer = vertexBuffer;
	}
	void OpenGLVertexArray::SetElementBuffer(const Ref<ElementBuffer>& elementBuffer)
	{
		m_ElementBuffer = elementBuffer;
	}
	void OpenGLVertexArray::Load()
	{
		BufferLayout layout = m_VertexBuffer->GetBufferLayout();
		std::vector<BufferElement> buffer_elements = layout.GetBufferElements();

		uint8_t stride_count = 0;

		for (const BufferElement& element : buffer_elements)
			stride_count += element.count_per_vertex;

		m_VertexBuffer->Bind();
		m_ElementBuffer->Bind();

		for (const BufferElement& element : buffer_elements)
		{
			switch (element.type)
			{
			case BufferDataType::Float:
				glVertexAttribPointer(element.shader_location,
					element.count_per_vertex, GL_FLOAT,
					element.normalized ? GL_TRUE : GL_FALSE,
					stride_count * sizeof(float),
					(const void*)(element.offset_count * sizeof(float)));
				break;

			case BufferDataType::Int:
				glVertexAttribPointer(element.shader_location,
					element.count_per_vertex, GL_INT,
					element.normalized ? GL_TRUE : GL_FALSE,
					stride_count * sizeof(int),
					(const void*)(element.offset_count * sizeof(int)));
				break;
			
			case BufferDataType::UnsignedInt:
				glVertexAttribPointer(element.shader_location,
					element.count_per_vertex, GL_UNSIGNED_INT,
					element.normalized ? GL_TRUE : GL_FALSE,
					stride_count * sizeof(unsigned int),
					(const void*)(element.offset_count * sizeof(unsigned int)));
				break;
			}
			glEnableVertexAttribArray(element.shader_location);
		}
	}
	inline void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_VertexArrayID);
	}
	inline void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
}