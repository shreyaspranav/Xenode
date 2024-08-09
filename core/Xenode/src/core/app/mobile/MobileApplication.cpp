#include <pch>
#include "MobileApplication.h"
#include <core/app/Timer.h>

#ifdef XEN_DEVICE_MOBILE
namespace Xen {
	MobileApplication::MobileApplication()
	{
		m_Api = GraphicsAPI::XEN_OPENGLES_API;
		stack = std::make_unique<LayerStack>(20);
	}

	MobileApplication::~MobileApplication()
	{
	}

	void MobileApplication::PushLayer(const Ref<Layer>& layer) { stack->PushLayer(layer); }
	void MobileApplication::PushLayer(const Ref<Layer>& layer, uint8_t loc) { stack->PushLayer(layer, loc); }
	void MobileApplication::PopLayer() { stack->PopLayer(); }
	void MobileApplication::PopLayer(uint8_t loc) { stack->PopLayer(loc); }

	void MobileApplication::OnCreate()
	{
		OnCreate();
	}
	void MobileApplication::OnStart()
	{
		m_Surface = Surface::GetSurface(m_ApplicationContext);
		OnStart();
	}
	void MobileApplication::OnUpdate(double timestep)
	{
		m_Surface->Update();

		OnUpdate(timestep);
		for(int i = stack->GetCount(); i >=1; i--)
			stack->GetLayer(i)->OnUpdate(timestep);
	}

	void MobileApplication::ImGuiRender()
	{

	}

	void MobileApplication::OnRender()
	{
		OnRender();
		for (int i = stack->GetCount(); i >= 1; i--) { stack->GetLayer(i)->OnRender(); }
	}

	void MobileApplication::OnFixedUpdate()
	{
		OnFixedUpdate();
		for (int i = stack->GetCount(); i >= 1; i--) { stack->GetLayer(i)->OnFixedUpdate(); }
	}

	void MobileApplication::Run()
	{
		const double S_PER_UPDATE = 1.0 / 60.0;

		MobileApplication::OnCreate();
		MobileApplication::OnStart();

		double previous = Timer::GetTimeMS();
		double lag = 0.0;

		while (is_Running) {
			double current = Timer::GetTimeMS();
			double timestep = current - previous;
			previous = current;
		
			MobileApplication::OnUpdate(timestep);
			lag += timestep;
		
			while (lag >= S_PER_UPDATE * 1000.0)
			{
				MobileApplication::OnFixedUpdate();
				lag -= S_PER_UPDATE * 1000.0;
			}
			// Run this function in a different thread:
			MobileApplication::OnRender();
			ImGuiRender();
		}

		for(int i = stack->GetCount(); i >=1; i--)
		{
			stack->GetLayer(i)->OnDetach();
		}
	}
}
#endif // XEN_DEVICE_MOBILE