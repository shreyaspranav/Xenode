#include "pch"
#include "QueryObject.h"

#include <core/app/GameApplication.h>

#include <gfxapi/OpenGL/OpenGLQueryObject.h>

namespace Xen
{
	Ref<QueryObject> QueryObject::CreateQueryObject(QueryTarget target)
	{
		switch (GetApplicationInstance()->GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLQueryObject>(target);
		}
		return nullptr;
	}
}