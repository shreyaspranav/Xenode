#pragma once

#ifdef XEN_DEVICE_MOBILE
#include <Core.h>
#include "Layer.h"
#include "LayerStack.h"
#include "GraphicsAPI.h"
#include "Surface.h"

namespace Xen {

	class XEN_API MobileApplication
	{
	private:
		void* m_ApplicationContext;
		inline static GraphicsAPI m_Api;
		Scope<LayerStack> stack;
		bool is_Running = 1;

		Ref<Surface> m_Surface;

	public:
		MobileApplication();
		virtual ~MobileApplication();

		void SetApplicationContext(void* context) { m_ApplicationContext = context; }
		void Run();

		void PushLayer(const Ref<Layer>& layer);
		void PushLayer(const Ref<Layer>& layer, uint8_t loc);

		void PopLayer();
		void PopLayer(uint8_t loc);

		virtual void OnCreate();
		virtual void OnStart();
		virtual void OnUpdate(double timestep);
		virtual void OnFixedUpdate();
		virtual void OnRender();

		virtual void ImGuiRender();

		static inline void SetGraphicsAPI(GraphicsAPI api) { m_Api = api; }
		static inline GraphicsAPI GetGraphicsAPI()         { return m_Api; }
	};

	XEN_API MobileApplication* CreateMobileApplication();
}
#endif
