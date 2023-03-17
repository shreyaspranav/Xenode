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
	void OnMouseMoveEvent(Xen::MouseMoveEvent& event) override;
	void OnMouseButtonPressEvent(Xen::MouseButtonPressEvent& event) override;
	void OnMouseButtonReleaseEvent(Xen::MouseButtonReleaseEvent& event) override;
	
	void OnKeyPressEvent(Xen::KeyPressEvent& event) override;

private:
	double m_Timestep;

	uint32_t viewport_framebuffer_width = 1, viewport_framebuffer_height = 1;

	Xen::Ref<Xen::FrameBuffer> m_ViewportFrameBuffer;
	Xen::Ref<Xen::Scene> m_ActiveScene;

	SceneHierarchyPanel hier_panel;
	PropertiesPanel prop_panel; 

	GizmoOperation m_GizmoOperation;

	// Editor Camera Stuff------------------------------------
	Xen::Ref<Xen::Camera> m_EditorCamera;

	Xen::Vec3 m_FocalPoint;
	Xen::Vec3 m_FocalPointCurrent;

	Xen::Vec3 m_CameraPosition = Xen::Vec3(0.0f, 0.0f, -4.0f);
	Xen::Vec3 m_CameraRightPosition;
	Xen::Vec3 m_CameraUpPosition;

	Xen::Vec3 m_CameraPositionWhenClicked;

	Xen::Vec2 m_CameraRotationAlongFocalPoint = Xen::Vec2(90.0f, 0.0f);
	Xen::Vec2 m_CameraRotationAlongFocalPointCurrent;
	Xen::Vec2 m_CameraRotationAlongFocalPointWhenClicked;

	float m_DistanceToFocalPoint = 4.0f;

	Xen::Vec2 m_NormalizedViewportMouseCoordinates;
	Xen::Vec2 m_NormalizedViewportMouseCoordinatesWhenClicked;

	bool m_IsMouseHoveredOnViewport = 0;

	bool m_IsMouseScrolled = 0;

	bool m_IsOrbitKeyPressed = 0; // Middle Mouse
	bool m_IsRotateOver = 0;

	bool m_IsPanKeyPressed = 0; // LShift + Middle Mouse
	bool m_IsPanOver = 0;

	int8_t m_ScrollDir = 0;

	uint8_t zoom_iterations = 0;

	// Editor Camera Controls
	Xen::MouseKeyCode orbit_key = Xen::MouseKeyCode::MOUSE_BUTTON_3;
	Xen::KeyCode pan_key = Xen::KeyCode::KEY_LEFT_SHIFT;
	//----------------------------------------------------------

private:
	Xen::Vec3 GetCameraFrontDir();
};