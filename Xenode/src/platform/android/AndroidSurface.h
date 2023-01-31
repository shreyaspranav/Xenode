#pragma once

#ifdef XEN_PLATFORM_ANDROID
#include <core/app/Surface.h>
#include "android_native_app_glue.h"

namespace Xen {
	class XEN_API AndroidSurface : public Surface
	{
	public:
		AndroidSurface(void* application_context);
		virtual ~AndroidSurface();

		void Create() override;
		void Update() override;
		void Shutdown() override;

		void SetRenderingAPI(GraphicsAPI api) override {}

	private:
		android_app* m_ApplicationContext;
	};
}

#endif // XEN_PLATFORM_ANDROID
