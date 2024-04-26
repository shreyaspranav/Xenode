#pragma once

#include <Core.h>

namespace Xen 
{
	enum class QueryTarget { TransformFeedbackPrimitivesWritten };

	class XEN_API QueryObject
	{
	public:
		virtual void BeginQuery() const = 0;
		virtual void EndQuery() const = 0;

		virtual void GetQueryResult(const void* data) const = 0;

		static Ref<QueryObject> CreateQueryObject(QueryTarget target);
	};
}