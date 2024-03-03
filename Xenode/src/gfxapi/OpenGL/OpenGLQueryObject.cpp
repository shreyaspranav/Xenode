#include "pch"
#include "OpenGLQueryObject.h"

#include <core/app/Log.h>

#include <glad/gl.h>

namespace Xen 
{
	static GLenum ToGLQueryTarget(QueryTarget target)
	{
		switch (target)
		{
		case Xen::QueryTarget::TransformFeedbackPrimitivesWritten: return GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN;
		}

		XEN_ENGINE_LOG_ERROR("Unknown QueryTarget!");
		TRIGGER_BREAKPOINT;

		return (GLenum)0;
	}

	OpenGLQueryObject::OpenGLQueryObject(QueryTarget target)
		:m_QueryTarget(target)
	{
		glCreateQueries(ToGLQueryTarget(m_QueryTarget), 1, &m_QueryID);
	}
	OpenGLQueryObject::~OpenGLQueryObject()
	{
		glDeleteQueries(1, &m_QueryID);
	}
	void OpenGLQueryObject::BeginQuery() const
	{
		glBeginQuery(ToGLQueryTarget(m_QueryTarget), m_QueryID);
	}
	void OpenGLQueryObject::EndQuery() const
	{
		glEndQuery(ToGLQueryTarget(m_QueryTarget));
	}
	void OpenGLQueryObject::GetQueryResult(const void* data) const
	{
		glGetQueryObjectiv(m_QueryID, GL_QUERY_RESULT, (int32_t*)data);
	}
}
