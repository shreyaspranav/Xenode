#pragma once
#ifdef XEN_DEVICE_MOBILE

#include <Core.h>
#include "GraphicsAPI.h"

namespace Xen {
	class XEN_API Surface
	{
	public:
		virtual ~Surface() = default;

		virtual void Create() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;

		virtual void SetRenderingAPI(GraphicsAPI api) = 0;

		static Ref<Surface> GetSurface(void* application_context);
	};
}
#endif

