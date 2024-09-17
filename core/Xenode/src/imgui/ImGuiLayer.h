#pragma once

#include <core/app/Layer.h>
#include <Core.h>

#ifdef XEN_DEVICE_DESKTOP
#include <core/app/desktop/Window.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#include "IconsFontAwesome.h"

namespace Xen {
	class XEN_API ImGuiLayer : public Layer
	{
	private:
		Ref<Window> m_Window;
		bool m_FirstTime = true;
	public:
		ImGuiLayer() {}
		virtual ~ImGuiLayer() {}

		void SetWindow(const Ref<Window>& window) { m_Window = window; }

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float timestep) override;

		virtual void OnImGuiUpdate() override;

		void Begin();
		void End();
	};
}

