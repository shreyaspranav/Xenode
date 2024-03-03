#include "pch"
#include "QueryObject.h"

#include <gfxapi/OpenGL/OpenGLQueryObject.h>

#include <core/app/DesktopApplication.h>

namespace Xen
{
	Ref<QueryObject> QueryObject::CreateQueryObject(QueryTarget target)
	{
		switch (DesktopApplication::GetGraphicsAPI())
		{
		case GraphicsAPI::XEN_OPENGL_API:
			return std::make_shared<OpenGLQueryObject>(target);
		}
		return nullptr;
	}
}