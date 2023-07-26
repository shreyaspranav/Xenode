#include "pch"
#include "OpenGLVertexArray.h"

#include <glad/gl.h>

#include "core/app/Profiler.h"

namespace Xen {
	OpenGLVertexArray::OpenGLVertexArray()
	{
		glGenVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);

		m_StrideCount = 0;
	}
	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_VertexArrayID);
	}
	void OpenGLVertexArray::SetVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		m_VertexBuffer = vertexBuffer;
	}
	void OpenGLVertexArray::SetElementBuffer(const Ref<ElementBuffer>& elementBuffer)
	{
		m_ElementBuffer = elementBuffer;
	}
	void OpenGLVertexArray::Load(bool hasIndexBuffer)
	{
		XEN_PROFILE_FN();

		/*
		VertexBufferLayout layout = m_VertexBuffer->GetBufferLayout();
		std::vector<VertexBufferElement> buffer_elements = layout.GetBufferElements();

		for (const VertexBufferElement& element : buffer_elements)
			m_StrideCount += element.count_per_vertex;

		m_VertexBuffer->Bind();
		if(hasIndexBuffer)
			m_ElementBuffer->Bind();

		for (const VertexBufferElement& element : buffer_elements)
		{
			switch (element.type)
			{
			case BufferDataType::Float:
				glVertexAttribPointer(element.shader_location,
					element.count_per_vertex, GL_FLOAT,
					element.normalized ? GL_TRUE : GL_FALSE,
					m_StrideCount * sizeof(float),
					(const void*)(element.offset_count * sizeof(float)));
				break;

			case BufferDataType::Int:
				glVertexAttribPointer(element.shader_location,
					element.count_per_vertex, GL_INT,
					element.normalized ? GL_TRUE : GL_FALSE,
					m_StrideCount * sizeof(int),
					(const void*)(element.offset_count * sizeof(int)));
				break;
			
			case BufferDataType::UnsignedInt:
				glVertexAttribPointer(element.shader_location,
					element.count_per_vertex, GL_UNSIGNED_INT,
					element.normalized ? GL_TRUE : GL_FALSE,
					m_StrideCount * sizeof(unsigned int),
					(const void*)(element.offset_count * sizeof(unsigned int)));
				break;
			}
			glEnableVertexAttribArray(element.shader_location);
		}
		*/
	}
	inline void OpenGLVertexArray::Bind() const
	{
		XEN_PROFILE_FN();

		glBindVertexArray(m_VertexArrayID);
	}
	inline void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}
}