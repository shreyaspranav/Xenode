#pragma once

#include <Xenode.h>
#include "panel/SceneHierarchyPanel.h"
#include "panel/PropertiesPanel.h"
#include "panel/ContentBrowserPanel.h"
#include "panel/SceneSettingsPanel.h"

#include "core/scene/SceneSerializer.h"
#include "core/scene/EditorCameraController.h"

enum KeyTransformOperation : uint16_t
{
	None = 0,

	TranslateX = BIT(0),
	TranslateY = BIT(1),
	TranslateZ = BIT(2),
	RotateX = BIT(3),
	RotateY = BIT(4),
	RotateZ = BIT(5),
	ScaleX = BIT(6),
	ScaleY = BIT(7),
	ScaleZ = BIT(8),

	Translate = TranslateX | TranslateY | TranslateZ,
	Rotate = RotateX | RotateY | RotateZ,
	Scale = ScaleX | ScaleY | ScaleZ,

	Translate2D = TranslateX | TranslateY,
	Rotate2D = RotateZ,
	Scale2D = ScaleX | ScaleY
};

class EditorLayer : public Xen::Layer
{
public:
	enum class GameMode { _2D, _3D };

	enum class GizmoOperation
	{
		Translate,
		Rotate2D, Rotate3D,
		Scale
	};

	enum class EditorState { Play, Edit, Pause };

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

	void OpenScene(const std::string& filePath);
	void SaveScene(const std::string& filePath);

	void OnWindowResizeEvent(Xen::WindowResizeEvent& event) override;

	void OnMouseScrollEvent(Xen::MouseScrollEvent& event) override;
	void OnMouseMoveEvent(Xen::MouseMoveEvent& event) override;
	void OnMouseButtonPressEvent(Xen::MouseButtonPressEvent& event) override;
	void OnMouseButtonReleaseEvent(Xen::MouseButtonReleaseEvent& event) override;
	
	void OnKeyPressEvent(Xen::KeyPressEvent& event) override;

private:
	double m_Timestep;
	GameMode m_GameMode = GameMode::_2D;

	uint32_t viewport_framebuffer_width = 1, viewport_framebuffer_height = 1;

	Xen::Ref<Xen::FrameBuffer> m_ViewportFrameBuffer;
	Xen::Ref<Xen::Scene> m_ActiveScene;
	Xen::Ref<Xen::Scene> m_EditorScene, m_RuntimeScene;

	// Panels:
	SceneHierarchyPanel m_HierarchyPanel;
	SceneSettingsPanel m_SceneSettingsPanel;
	PropertiesPanel m_PropertiesPanel; 
	ContentBrowserPanel m_ContentBrowserPanel;

	GizmoOperation m_GizmoOperation;
	KeyTransformOperation m_KeyTransformOperation = KeyTransformOperation::None;

	bool m_IsMouseHoveredOnViewport;
	bool m_ScenePaused = false;
	bool m_SceneStepped = false;

	// To disable mouse picking when mouse is over a gizmo:
	bool m_IsMousePickingWorking = true;

	Xen::Vec2 viewport_mouse_pos;

	// Editor Camera Stuff------------------------------------
	Xen::Ref<Xen::Camera> m_EditorCamera;
	Xen::EditorCameraController m_EditorCameraController;

	Xen::Ref<Xen::Texture2D> m_PlayTexture;
	Xen::Ref<Xen::Texture2D> m_PauseTexture;
	Xen::Ref<Xen::Texture2D> m_StopTexture;
	Xen::Ref<Xen::Texture2D> m_StepTexture;

	EditorState m_EditorState = EditorState::Edit;

	Xen::Ref<Xen::Texture2D> m_PlayOrPause;
	bool m_EditMode = true;
};