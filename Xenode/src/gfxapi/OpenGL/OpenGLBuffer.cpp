#include "pch"
#include "OpenGLBuffer.h"
#include "core/app/Log.h"

#include <glad/gl.h>

namespace Xen {

	//-------OpenGLFloatBuffer---------------------------------------------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	OpenGLFloatBuffer::OpenGLFloatBuffer(uint32_t count) : m_Count(count), m_Size(count * sizeof(float))
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
	}
	OpenGLFloatBuffer::~OpenGLFloatBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}
	void OpenGLFloatBuffer::Put(float* data, uint32_t count)
	{
		if (count * sizeof(float) > m_Size)
		{
			XEN_ENGINE_LOG_ERROR("The specified array of data is larger than specified!");
			TRIGGER_BREAKPOINT;
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), data, GL_DYNAMIC_DRAW);
	}
	void OpenGLFloatBuffer::Put(uint32_t offsetCount, float* data, uint32_t count)
	{
		uint32_t available_count = m_Count - offsetCount;

		if(available_count > count)
		{
			XEN_ENGINE_LOG_ERROR("The specified array of data is larger than specified!");
			TRIGGER_BREAKPOINT;
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ARRAY_BUFFER, offsetCount * sizeof(float), count * sizeof(float), data);
	}
	inline void OpenGLFloatBuffer::Bind()					{ glBindBuffer(GL_ARRAY_BUFFER, m_BufferID); }
	inline void OpenGLFloatBuffer::Unbind()					{ glBindBuffer(GL_ARRAY_BUFFER, 0); }
	inline uint32_t OpenGLFloatBuffer::GetCount()			{ return m_Count; }
	inline uint32_t OpenGLFloatBuffer::GetSize()			{ return m_Size; }

	//-------OpenGLElementBuffer---------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	OpenGLElementBuffer::OpenGLElementBuffer(uint32_t count) : m_Count(count), m_Size(count * sizeof(float))
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
	}
	OpenGLElementBuffer::~OpenGLElementBuffer()
	{
		glDeleteBuffers(1, &m_BufferID);
	}
	void OpenGLElementBuffer::Put(uint32_t* data, uint32_t count)
	{
		if (count * sizeof(float) > m_Size)
		{
			XEN_ENGINE_LOG_ERROR("The specified array of data is larger than specified!");
			TRIGGER_BREAKPOINT;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_DYNAMIC_DRAW);
	}
	void OpenGLElementBuffer::Put(uint32_t offsetCount, uint32_t* data, uint32_t count)
	{
		uint32_t available_count = m_Count - offsetCount;

		if (available_count > count)
		{
			XEN_ENGINE_LOG_ERROR("The specified array of data is larger than specified!");
			TRIGGER_BREAKPOINT;
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetCount * sizeof(float), count * sizeof(float), data);
	}
	inline void OpenGLElementBuffer::Bind()					{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID); }
	inline void OpenGLElementBuffer::Unbind()				{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	inline uint32_t OpenGLElementBuffer::GetCount()			{ return m_Count; }
	inline uint32_t OpenGLElementBuffer::GetSize()			{ return m_Size; }
}