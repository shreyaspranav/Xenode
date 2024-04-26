#pragma once
#include <Core.h>

#include <core/renderer/QueryObject.h>

namespace Xen 
{
	class OpenGLQueryObject : public QueryObject
	{
	public:
		OpenGLQueryObject(QueryTarget target);
		virtual ~OpenGLQueryObject();

		void BeginQuery() const override;
		void EndQuery() const override;

		void GetQueryResult(const void* data) const override;
	private:
		uint32_t m_QueryID;
		QueryTarget m_QueryTarget;
	};
}

