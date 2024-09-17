#pragma once

#include <Xenode.h>
#include <core/scene/Scene.h>

class MainRuntimeLayer : public Xen::Layer
{
public:
	MainRuntimeLayer();
	virtual ~MainRuntimeLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float timestep) override;
	void OnFixedUpdate() override;
	void OnRender() override;

	void OnEvent(Xen::Event& event) override;

	void OnWindowResizeEvent(Xen::WindowResizeEvent& event);
private:
	Xen::Ref<Xen::Scene> m_CurrentScene;
	bool m_FirstIteration = true;
};