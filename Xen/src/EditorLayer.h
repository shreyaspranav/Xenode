#pragma once

#include <Xenode.h>
#include "panel/SceneHierarchyPanel.h"
#include "panel/PropertiesPanel.h"

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
	void OnKeyPressEvent(Xen::KeyPressEvent& event) override;

private:
	double m_Timestep;

	uint32_t viewport_framebuffer_width = 1, viewport_framebuffer_height = 1;

	Xen::Ref<Xen::Camera> m_EditorCamera;
	Xen::Ref<Xen::Camera> m_SecondCamera;
	Xen::Ref<Xen::FrameBuffer> m_ViewportFrameBuffer;

	Xen::Ref<Xen::Scene> m_ActiveScene;

	Xen::Ref<Xen::Texture2D> tex;
	Xen::Entity quad_entity;
	Xen::Entity quad_entity_1;
	Xen::Entity camera_entity;

	float cam_zoom = 1.0f;
	float max_zoom = 20.0f;
	float min_zoom = 0.3f;

	bool m_ViewportFocused = 0;

	SceneHierarchyPanel hier_panel;
	PropertiesPanel prop_panel;
};