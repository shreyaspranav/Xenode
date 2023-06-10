#pragma once

#include <Xenode.h>
#include "panel/SceneHierarchyPanel.h"
#include "panel/PropertiesPanel.h"
#include "panel/ContentBrowserPanel.h"

#include "core/scene/SceneSerializer.h"
#include "core/scene/EditorCameraController.h"

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

	void OnScenePlay();
	void OnSceneStop();
	void OnScenePause();

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
	Xen::Ref<Xen::Scene> m_EditorScene, m_RuntimeScene;

	// Panels:
	SceneHierarchyPanel m_HierarchyPanel;
	PropertiesPanel m_PropertiesPanel; 
	ContentBrowserPanel m_ContentBrowserPanel;

	GizmoOperation m_GizmoOperation;

	bool m_IsMouseHoveredOnViewport;
	Xen::Vec2 viewport_mouse_pos;

	// Editor Camera Stuff------------------------------------
	Xen::Ref<Xen::Camera> m_EditorCamera;
	Xen::EditorCameraController m_EditorCameraController;

	Xen::Ref<Xen::Texture2D> m_PlayTexture;
	Xen::Ref<Xen::Texture2D> m_PauseTexture;
	Xen::Ref<Xen::Texture2D> m_StopTexture;

	enum class EditorState { Play, Edit, Pause };

	EditorState m_EditorState = EditorState::Edit;

	Xen::Ref<Xen::Texture2D> m_PlayOrPause;
	bool m_EditMode = true;

private:
	Xen::Vec3 GetCameraFrontDir();
};