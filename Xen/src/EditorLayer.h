#pragma once

#include <Xenode.h>
#include "panel/SceneHierarchyPanel.h"
#include "panel/PropertiesPanel.h"

#include "core/scene/SceneSerializer.h"

class EditorLayer : public Xen::Layer
{
public:
	enum class GizmoOperation {
		Translate,
		Rotate2D, Rotate3D,
		Scale
	};


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
	Xen::Ref<Xen::FrameBuffer> m_ViewportFrameBuffer;
	Xen::Ref<Xen::Scene> m_ActiveScene;

	float editor_cam_zoom = 1.0f;
	float editor_max_zoom = 20.0f;
	float editor_min_zoom = 0.3f;

	SceneHierarchyPanel hier_panel;
	PropertiesPanel prop_panel; 

	GizmoOperation m_GizmoOperation;

	bool m_IsMouseHoveredOnViewport = 0;
};