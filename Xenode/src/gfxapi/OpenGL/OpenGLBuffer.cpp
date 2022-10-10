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
	inline void OpenGLFloatBuffer::Bind() const				{ glBindBuffer(GL_ARRAY_BUFFER, m_BufferID); }
	inline void OpenGLFloatBuffer::Unbind()	const			{ glBindBuffer(GL_ARRAY_BUFFER, 0); }
	inline uint32_t OpenGLFloatBuffer::GetCount() const		{ return m_Count; }
	inline uint32_t OpenGLFloatBuffer::GetSize() const		{ return m_Size; }

	//-------OpenGLElementBuffer---------------------------------------------------------------------------------
	//-----------------------------------------------------------------------------------------------------------

	OpenGLElementBuffer::OpenGLElementBuffer(uint32_t count) : m_Count(count), m_Size(count * sizeof(float))
	{
		glGenBuffers(1, &m_BufferID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);

		m_ActiveCount = 0;
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Size, data, GL_DYNAMIC_DRAW);

		m_ActiveCount += count;
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
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offsetCount * sizeof(uint32_t), count * sizeof(uint32_t), data);

		m_ActiveCount += count;
	}
	inline void OpenGLElementBuffer::Bind() const					{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID); }
	inline void OpenGLElementBuffer::Unbind() const					{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	inline uint32_t OpenGLElementBuffer::GetCount() const			{ return m_Count; }
	inline uint32_t OpenGLElementBuffer::GetActiveCount() const		{ return m_ActiveCount; }

	inline uint32_t OpenGLElementBuffer::GetSize() const	{ return m_Size; }
}