#pragma once

#include <Xenode.h>
#include "panel/SceneHierarchyPanel.h"
#include "panel/PropertiesPanel.h"
#include "panel/ContentBrowserPanel.h"
#include "panel/SceneSettingsPanel.h"

#include <core/scene/SceneSerializer.h>
#include <core/scene/EditorCameraController.h>

#include <core/scene/SceneRuntime.h>

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

class LevelEditorLayer : public Xen::Layer
{
public:
	enum class GizmoOperation
	{
		Translate,
		Rotate2D, Rotate3D,
		Scale
	};

	enum class EditorState { Play, Edit, Pause };

	LevelEditorLayer();
	virtual ~LevelEditorLayer();

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float timestep) override;
	void OnRender() override;
	void OnImGuiUpdate() override;
	void OnFixedUpdate() override;

	void OnEvent(Xen::Event& event) override;

	void OnScenePlay();
	void OnSceneStop();
	void OnScenePause();

	void OpenScene(const std::string& filePath);
	void SaveScene(const std::string& filePath);

	void OnWindowResizeEvent(Xen::WindowResizeEvent& event);

	void OnMouseScrollEvent(Xen::MouseScrollEvent& event);
	void OnMouseMoveEvent(Xen::MouseMoveEvent& event);
	void OnMouseButtonEvent(Xen::MouseButtonEvent& event);
	
	void OnKeyboardEvent(Xen::KeyboardEvent& event);
private:
	// The OnImGuiUpdate function is broken down into smaller functions:
	void ImGuiSetupDockSpace();
	void ImGuiRenderMenuBar();
	void ImGuiRenderPanels();

	void ImGuiRenderViewport();
		void ImGuiRenderOverlay(); // ImGuiRenderViewport() calls ImGuiRenderOverlay()

	void ImGuiRenderToolbar();

private:
	float m_Timestep;
	Xen::GameType m_GameMode;
	Xen::EditorCameraType m_EditorCameraType = Xen::EditorCameraType::_2D;

	uint32_t m_ViewportFrameBufferWidth = 1, m_ViewportFrameBufferHeight = 1;

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

	Xen::Vec2 m_ViewportMousePos;

	// Editor Camera Stuff------------------------------------
	Xen::Ref<Xen::Camera> m_EditorCamera;
	Xen::EditorCameraController m_EditorCameraController;

	// Resource textures refers to textures that are used in the editor:
	Xen::UnorderedMap<std::string, Xen::Ref<Xen::Texture2D>> m_ResourceTextures;

	EditorState m_EditorState = EditorState::Edit;
	Xen::SceneSettings m_SceneSettings;

	Xen::Ref<Xen::Texture2D> m_PlayOrPause;
	Xen::Ref<Xen::Texture2D> m_2DOr3DView;
	bool m_EditMode = true;
	bool m_FirstRuntimeIteration = false;

	Xen::Entity m_SelectedEntity;
};