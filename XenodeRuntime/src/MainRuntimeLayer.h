#pragma once

#include <Xenode.h>

class MainRuntimeLayer : public Xen::Layer
{
public:
	MainRuntimeLayer();
	virtual ~MainRuntimeLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(double timestep) override;
	void OnFixedUpdate() override;
	void OnRender() override;

	void OnWindowResizeEvent(Xen::WindowResizeEvent& event) override;
private:
	Xen::Ref<Xen::Scene> m_CurrentScene;
};