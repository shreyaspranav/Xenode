#pragma once

#include <Xenode.h>

class EditorLayer : public Xen::Layer
{
public:
	EditorLayer();
	virtual ~EditorLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(double timestep) override;
	void OnImGuiUpdate() override;
	void OnFixedUpdate() override;

	void OnWindowResizeEvent(Xen::WindowResizeEvent& event) override;
	void OnMouseScrollEvent(Xen::MouseScrollEvent& event) override;
private:
	double m_Timestep;

	uint32_t viewport_framebuffer_width = 0, viewport_framebuffer_height = 0;

	Xen::Ref<Xen::OrthographicCamera> m_EditorCamera;
	Xen::Ref<Xen::FrameBuffer> m_ViewportFrameBuffer;

	Xen::Ref<Xen::Scene> m_ActiveScene;

	Xen::Ref<Xen::Texture2D> tex;
	Xen::Entity quad_entity;

	float cam_zoom = 1.0f;
	float max_zoom = 20.0f;
	float min_zoom = 0.3f;

	bool m_ViewportFocused = 0;
};