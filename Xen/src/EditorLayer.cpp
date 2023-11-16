#include "EditorLayer.h"
#include "core/scene/ScriptableEntity.h"

#include <core/app/Timer.h>
#include <core/app/Utils.h>

#include <ImGuizmo.h>

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "math/Math.h"
#include <glm/gtx/quaternion.hpp>

Xen::Ref<Xen::Input> input;

bool orbit_over = false;
Xen::Vec2 initial_pos;

float line_width = 1.0f;

bool operator&(const KeyTransformOperation& o1, const KeyTransformOperation& o2) { return static_cast<uint16_t>(o1) & static_cast<uint16_t>(o2); }

EditorLayer::EditorLayer()
{
	m_Timestep = 0.0f;
}

EditorLayer::~EditorLayer()
{
}

void EditorLayer::OnAttach()
{
	input = Xen::Input::GetInputInterface();
	input->SetWindow(Xen::DesktopApplication::GetWindow());

	m_EditorCamera = std::make_shared<Xen::Camera>(
		m_GameMode == GameMode::_2D ? Xen::CameraType::Orthographic : Xen::CameraType::Perspective, 
		viewport_framebuffer_width, 
		viewport_framebuffer_height
	);

	Xen::Renderer2D::Init();

	m_EditorScene = std::make_shared<Xen::Scene>();
	m_RuntimeScene = std::make_shared<Xen::Scene>();
	m_ActiveScene = m_EditorScene;

	m_EditorCamera->Update();

	// To add native scripts to the entities
	//class CameraControlScript : public Xen::ScriptableEntity
	//{
	//private:
	//	Xen::Ref<Xen::Input> input;
	//public:
	//	void OnCreate() override{
	//		input = GetInput();
	//	}
	//
	//	void OnUpdate(double timestep) override{
	//		
	//	}
	//
	//	void OnDestroy() override{
	//
	//	}
	//};
	//quad_entity_1.AddComponent<Xen::Component::NativeScript>().Bind<CameraControlScript>(quad_entity_1);

	m_HierarchyPanel = SceneHierarchyPanel(m_ActiveScene);
	m_PropertiesPanel = PropertiesPanel(m_HierarchyPanel.GetSelectedEntity());
	m_ContentBrowserPanel = ContentBrowserPanel();
	m_SceneSettingsPanel = SceneSettingsPanel();

	m_PropertiesPanel.SetTextureLoadDropType(m_ContentBrowserPanel.GetTextureLoadDropType());
	m_PropertiesPanel.SetScriptLoadDropType(m_ContentBrowserPanel.GetScriptLoadDropType());

	m_EditorCameraController = Xen::EditorCameraController(input, Xen::EditorCameraType::_2D);


	m_PlayTexture = Xen::Texture2D::CreateTexture2D("assets/textures/play.png", false);
	m_StopTexture = Xen::Texture2D::CreateTexture2D("assets/textures/stop.png", false);
	m_PauseTexture = Xen::Texture2D::CreateTexture2D("assets/textures/pause.png", false);
	m_StepTexture = Xen::Texture2D::CreateTexture2D("assets/textures/step.png", false);

	m_PlayTexture->LoadTexture();
	m_StopTexture->LoadTexture();
	m_PauseTexture->LoadTexture();
	m_StepTexture->LoadTexture();

	// Assuming that m_EditorState is m_EditorState::Edit in the beginning
	m_PlayOrPause = m_PlayTexture;

	m_EditorScene->Test();
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(double timestep)
{

	Xen::Vec2 mouse = Xen::Vec2(input->GetMouseX(), input->GetMouseY());
	Xen::Vec2 delta = (mouse - initial_pos) * 0.3f;
	initial_pos = mouse;

	bool active = true;

	Xen::RenderCommand::Clear();
	m_Timestep = timestep;

	if (m_EditorState == EditorState::Edit)
	{
		m_EditorCameraController.Update(&active);
		m_EditorCamera->SetPosition(m_EditorCameraController.GetCameraPosition());

		if (m_GameMode == GameMode::_2D) 
		{
			m_EditorCamera->SetScale({ m_EditorCameraController.GetFocalDistance(), m_EditorCameraController.GetFocalDistance(), 1.0f });
			m_EditorCamera->Update(false);
		}

		else if (m_GameMode == GameMode::_3D)
		{
			m_EditorCamera->LookAtPoint(m_EditorCameraController.GetFocalPoint());
			m_EditorCamera->Update(true);
		}


		m_ActiveScene->OnUpdate(timestep, m_EditorCamera);

		m_EditorScene = m_ActiveScene;
	}

	else if (m_EditorState == EditorState::Play || m_EditorState == EditorState::Pause) 
	{
		m_ActiveScene = m_RuntimeScene;

		if (m_SceneStepped) {
			m_ActiveScene->OnUpdateRuntime(timestep, false);
			m_SceneStepped = false;
		}
		else 
			m_ActiveScene->OnUpdateRuntime(timestep, m_ScenePaused);

	}

	// Line Rendering Test

	// Xen::RenderCommand::SetLineWidth(1.0f);
	// 
	// for (int i = -5; i < 6; i++)
	// {
	// 	Xen::Renderer2D::DrawLine(Xen::Vec3(-5.0f, i, 0.0f), Xen::Vec3(5.0f, i, 0.0f), Xen::Color(0.9f, 0.9f, 0.9f, 1.0f));
	// 	Xen::Renderer2D::DrawLine(Xen::Vec3(i, -5.0f, 0.0f), Xen::Vec3(i, 5.0f, 0.0f), Xen::Color(0.9f, 0.9f, 0.9f, 1.0f));
	// }

	if (input->IsMouseButtonPressed(Xen::MOUSE_BUTTON_LEFT) && m_IsMouseHoveredOnViewport && m_IsMousePickingWorking)
	{
		// For some reason the red integer attachment is flipped!
		int entt_id = m_ActiveScene->GetUnlitSceneFrameBuffer()->ReadIntPixel(m_ActiveScene->GetMousePickingFrameBufferIndex(),
			viewport_mouse_pos.x, 
			viewport_framebuffer_height - viewport_mouse_pos.y);

		m_HierarchyPanel.SetSelectedEntity(Xen::Entity((entt::entity)entt_id, m_ActiveScene.get()));
	}

	m_ActiveScene->OnRender();
} 

void EditorLayer::OnImGuiUpdate()
{
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, window_flags);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

		static auto first_time = true;
		if (first_time)
		{
			first_time = false;

			ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
			ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

			// split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
			// window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id 
			// we want (which ever one we DON'T set as NULL, will be returned by the function)
			// out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction

			auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.22f, nullptr, &dockspace_id); // For the Scene hierarchy panel and properties panel
			auto dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.3f, nullptr, &dockspace_id);  // For the Content Browser panel
			auto dock_id_up = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.065f, nullptr, &dockspace_id);    // For the toolbar

			auto dock_id_left_down = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.5f, nullptr, &dock_id_left);

			// we now dock our windows into the docking node we made above
			ImGui::DockBuilderDockWindow(m_HierarchyPanel.GetPanelTitle().c_str(), dock_id_left);
			ImGui::DockBuilderDockWindow(m_ContentBrowserPanel.GetPanelTitle().c_str(), dock_id_down);
			ImGui::DockBuilderDockWindow("##toolbar", dock_id_up);
			ImGui::DockBuilderDockWindow(m_SceneSettingsPanel.GetPanelTitle().c_str(), dock_id_left_down);
			ImGui::DockBuilderDockWindow(m_PropertiesPanel.GetPanelTitle().c_str(), dock_id_left_down);
			//ImGui::DockBuilderDockWindow("Window Three", dock_id_left_down);
			ImGui::DockBuilderDockWindow((std::string(ICON_FA_MOUNTAIN_SUN) + std::string("  2D Viewport")).c_str(), dockspace_id); // IMP: To Dock In Centre!! use directly 'dockspace_id'
			ImGui::DockBuilderFinish(dockspace_id);
		}
	}
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Xenode", NULL, false, false);
			if (ImGui::MenuItem("New"))
			{
				m_EditorScene->NewScene();
				m_HierarchyPanel.SetActiveScene(m_ActiveScene);
			}
			if (ImGui::MenuItem("Open", "Ctrl+O")) 
			{
				const std::string& filePath = Xen::Utils::OpenFileDialogOpen("Xenode 2D Scene (*.xen)\0*.*\0");
				
				if (!filePath.empty())
				{
					OpenScene(filePath);
					m_ActiveScene = m_EditorScene;

					m_HierarchyPanel.SetActiveScene(m_ActiveScene);
				}
			}

			if (ImGui::MenuItem("Save", "Ctrl+S")) 
			{
				const std::string& filePath = Xen::Utils::OpenFileDialogSave("Xenode 2D Scene (*.xen)\0*.*\0");

				if (!filePath.empty())
					SaveScene(filePath);

			}
			if (ImGui::MenuItem("Save As..")) {}

			ImGui::Separator();

			if (ImGui::BeginMenu("Options"))
			{
				static bool b = true;
				ImGui::Checkbox("Auto Save", &b);
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Quit", "Alt+F4")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Project"))	{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("View"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Build"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("Tools"))		{ ImGui::EndMenu(); }
		if (ImGui::BeginMenu("About"))		{ ImGui::EndMenu(); }
		ImGui::EndMenuBar();
	}

	ImGui::End();

	m_HierarchyPanel.OnImGuiRender();

	m_ContentBrowserPanel.OnImGuiEditor();

	m_SceneSettingsPanel.OnImGuiRender();

	m_PropertiesPanel.OnImGuiRender();

	if(m_EditorState == EditorState::Edit)
		m_PropertiesPanel.SetActiveEntity(m_HierarchyPanel.GetSelectedEntity());
	else 
		m_PropertiesPanel.SetActiveEntity(m_ActiveScene->GetRuntimeEntity(m_HierarchyPanel.GetSelectedEntity(), m_ActiveScene));

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin((std::string(ICON_FA_MOUNTAIN_SUN) + std::string("  2D Viewport")).c_str());

	m_IsMouseHoveredOnViewport = ImGui::IsWindowHovered();

	float y_offset = ImGui::GetWindowHeight() - viewport_framebuffer_height;

	auto[sx, sy] = ImGui::GetCursorScreenPos();
	auto[mx, my] = ImGui::GetMousePos();

	viewport_mouse_pos.x = mx - sx;
	viewport_mouse_pos.y = my - sy;

	if (viewport_framebuffer_width != ImGui::GetContentRegionAvail().x || viewport_framebuffer_height != ImGui::GetContentRegionAvail().y)
	{
		if (ImGui::GetContentRegionAvail().x <= 0 || ImGui::GetContentRegionAvail().y <= 0) {}
		else {
			viewport_framebuffer_width = ImGui::GetContentRegionAvail().x;
			viewport_framebuffer_height = ImGui::GetContentRegionAvail().y;

			m_ActiveScene->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);
			m_EditorCamera->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);
		}
	}

	//m_ActiveScene->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);

	// Xen::Texture2D::BindTexture(m_ActiveScene->GetTestTexture(), 0);

	ImGui::Image((void*)m_ActiveScene->GetUnlitSceneFrameBuffer()->GetColorAttachmentRendererID(0), ImVec2(viewport_framebuffer_width, viewport_framebuffer_height), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::Image((void*)m_ActiveScene->GetSceneFrameBuffer()->GetColorAttachmentRendererID(0), ImVec2(viewport_framebuffer_width, viewport_framebuffer_height), ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(m_ContentBrowserPanel.GetSceneLoadDropType().c_str()))
		{
			if (m_EditorState != EditorState::Edit)
				m_EditorState = EditorState::Edit;

			std::string path = (const char*)payload->Data;
			uint32_t size = path.size();

			OpenScene(path);
			m_ActiveScene = m_EditorScene;

			m_HierarchyPanel.SetActiveScene(m_ActiveScene);
		}
		ImGui::EndDragDropTarget();
	}

	m_IsMousePickingWorking = true;

	if (m_EditorState == EditorState::Edit)
	{
		Xen::Entity selectedEntity = m_HierarchyPanel.GetSelectedEntity();
		if (!selectedEntity.IsNull() && selectedEntity.IsValid())
		{
			if (ImGuizmo::IsOver())
				m_IsMousePickingWorking = false;

			
			ImGuizmo::SetOrthographic(m_EditorCamera->GetProjectionType() == Xen::CameraType::Orthographic ? true : false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + y_offset, viewport_framebuffer_width, viewport_framebuffer_height);

			glm::mat4 camera_view = m_EditorCamera->GetViewMatrix();
			glm::mat4 camera_projection = m_EditorCamera->GetProjectionMatrix();

			Xen::Component::Transform& entity_transform_comp = selectedEntity.GetComponent<Xen::Component::Transform>();

			glm::vec3 radians_rotation_vec = glm::vec3(glm::radians(entity_transform_comp.rotation.x),
					glm::radians(entity_transform_comp.rotation.y),
					glm::radians(entity_transform_comp.rotation.z));

			glm::mat4 entity_transform = glm::translate(glm::mat4(1.0f), entity_transform_comp.position.GetVec())
							* glm::toMat4(glm::quat(radians_rotation_vec))
							* glm::scale(glm::mat4(1.0f), entity_transform_comp.scale.GetVec());

			switch (m_GizmoOperation)
			{
			case GizmoOperation::Translate:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Rotate2D:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::ROTATE_Z, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Rotate3D:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::ROTATE_Z, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			case GizmoOperation::Scale:
				ImGuizmo::Manipulate(glm::value_ptr(camera_view), glm::value_ptr(camera_projection),
					ImGuizmo::SCALE, ImGuizmo::LOCAL, glm::value_ptr(entity_transform));
				break;
			}

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, rotation, scale, skew;
				glm::quat orientation;
				glm::vec4 perspective;
			
				glm::decompose(entity_transform, scale, orientation, translation, skew, perspective);
			
				rotation = glm::eulerAngles(orientation);
			
				rotation.x = glm::degrees(rotation.x);
				rotation.y = glm::degrees(rotation.y);
				rotation.z = glm::degrees(rotation.z);
			
				glm::vec3 delta_rotation = rotation - entity_transform_comp.rotation.GetVec();
			
				entity_transform_comp.rotation.x += delta_rotation.x;
				entity_transform_comp.rotation.y += delta_rotation.y;
				entity_transform_comp.rotation.z += delta_rotation.z;
			
				entity_transform_comp.position = translation;
				entity_transform_comp.scale = scale;
			}

			// Key Transform Operations:
			// TODO: Work on the speed of the translation thing:
			// TODO: Rotation for X and Y axis:

			float speed = 0.01f * m_EditorCameraController.GetFocalDistance();

			if (m_KeyTransformOperation != KeyTransformOperation::None)
			{
				m_IsMousePickingWorking = false;
				auto& transformComp = selectedEntity.GetComponent<Xen::Component::Transform>();
				if (m_KeyTransformOperation & KeyTransformOperation::TranslateX)
				{
					if (m_GameMode == GameMode::_2D)
						transformComp.position.x += m_EditorCameraController.GetMouseDelta().x * speed;
				}

				if (m_KeyTransformOperation & KeyTransformOperation::TranslateY)
				{
					if (m_GameMode == GameMode::_2D)
						transformComp.position.y -= m_EditorCameraController.GetMouseDelta().y * speed;
				}

				if (m_KeyTransformOperation & KeyTransformOperation::TranslateY)
				{

				}

				if (m_KeyTransformOperation & KeyTransformOperation::RotateZ)
					transformComp.rotation.z -= m_EditorCameraController.GetMouseDelta().y;

				if (m_KeyTransformOperation & KeyTransformOperation::ScaleX)
					transformComp.scale.x += m_EditorCameraController.GetMouseDelta().x * speed;
				if (m_KeyTransformOperation & KeyTransformOperation::ScaleY)
					transformComp.scale.y += m_EditorCameraController.GetMouseDelta().x * speed;

				if (m_KeyTransformOperation != KeyTransformOperation::None && input->IsMouseButtonPressed(Xen::MouseKeyCode::MOUSE_BUTTON_LEFT))
				{
					m_KeyTransformOperation = KeyTransformOperation::None;
					m_IsMousePickingWorking = true;
				}
			}
		}
	}

	ImGui::End();

	ImGuiWindowClass window_class;
	window_class.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoTabBar;
	ImGui::SetNextWindowClass(&window_class);

	ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
	ImGui::Begin("##toolbar", nullptr, toolbar_flags);
	
	ImGui::PushStyleColor(ImGuiCol_Button, { 0, 0, 0, 0 });

	const char* scene_state;
	switch (m_EditorState)
	{
	case EditorLayer::EditorState::Play:
		scene_state = "Play";
		break;
	case EditorLayer::EditorState::Edit:
		scene_state = "Edit";
		break;
	case EditorLayer::EditorState::Pause:
		scene_state = "Pause";
		break;
	default:
		break;
	}

	//ImGui::Text("Scene State: %s", scene_state); ImGui::SameLine();

	ImGuiStyle& style = ImGui::GetStyle();
	float width = 0.0f;
	width += 25.0f;			// Play/Pause Button
	width += style.ItemSpacing.x;
	width += 25.0f;			// Stop Button
	width += style.ItemSpacing.x;
	width += 25.0f;			// Step Button
	width += style.ItemSpacing.x;

	float avail = ImGui::GetContentRegionAvail().x;
	float off = (avail - width) * 0.5f;
	if (off > 0.0f)
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

	if (ImGui::ImageButton((ImTextureID)m_PlayOrPause->GetNativeTextureID(), { 25.0f, 25.0f }))
	{
		if (m_PlayOrPause == m_PlayTexture)
		{
			m_PlayOrPause = m_PauseTexture;
			OnScenePlay();
		}
		else {
			m_PlayOrPause = m_PlayTexture;
			m_EditorState = EditorState::Pause;

			OnScenePause();
		}

		m_EditMode = false;
	}

	ImGui::SameLine();

	ImGui::BeginDisabled(m_EditMode);

	if (ImGui::ImageButton((ImTextureID)m_StopTexture->GetNativeTextureID(), { 25.0f, 25.0f })) 
	{
		m_EditorState = EditorState::Edit;
		m_EditMode = true;
		m_PlayOrPause = m_PlayTexture;

		OnSceneStop();
	}

	ImGui::SameLine();

	ImGui::BeginDisabled(m_EditorState != EditorState::Pause);
	if (ImGui::ImageButton((ImTextureID)m_StepTexture->GetNativeTextureID(), { 25.0f, 25.0f }))
	{
		m_SceneStepped = true;
	}
	ImGui::EndDisabled();
	ImGui::EndDisabled();

	ImGui::PopStyleColor();

	ImGui::End();

	ImGui::PopStyleVar();

	//ImGui::Begin("Test");
	//ImGui::SliderFloat("Circle Thickness", &line_width, 0.0f, 1.0f);
	//ImGui::End();
}

void EditorLayer::OnFixedUpdate()
{

}

void EditorLayer::OnScenePlay()
{
	m_ScenePaused = false;

	if (m_EditorState != EditorState::Pause)
	{
		m_RuntimeScene = Xen::Scene::Copy(m_EditorScene);
		m_ActiveScene = m_RuntimeScene;
		m_ActiveScene->OnRuntimeStart();
	}

	m_HierarchyPanel.SetActiveScene(m_RuntimeScene);
	m_ActiveScene->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);

	m_EditorState = EditorState::Play;
}

void EditorLayer::OnSceneStop()
{
	m_ActiveScene->OnRuntimeStop();
	m_ActiveScene = m_EditorScene;

	m_HierarchyPanel.SetActiveScene(m_EditorScene);
}

void EditorLayer::OnScenePause()
{
	m_ScenePaused = true;
}

void EditorLayer::OpenScene(const std::string& filePath)
{
	m_EditorScene->NewScene();
	Xen::SceneSerializer::Deserialize(m_EditorScene, filePath);
}

void EditorLayer::SaveScene(const std::string& filePath)
{
	Xen::SceneSerializer::Serialize(m_EditorScene, filePath);
}

void EditorLayer::OnWindowResizeEvent(Xen::WindowResizeEvent& event)
{
	m_EditorCamera->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);
	m_ActiveScene->OnViewportResize(viewport_framebuffer_width, viewport_framebuffer_height);
}

void EditorLayer::OnMouseScrollEvent(Xen::MouseScrollEvent& event)
{

}

void EditorLayer::OnMouseMoveEvent(Xen::MouseMoveEvent& event)
{

}

void EditorLayer::OnMouseButtonPressEvent(Xen::MouseButtonPressEvent& event)
{

}

void EditorLayer::OnMouseButtonReleaseEvent(Xen::MouseButtonReleaseEvent& event)
{

}

void EditorLayer::OnKeyPressEvent(Xen::KeyPressEvent& event)
{
	if (m_IsMouseHoveredOnViewport)
	{
		switch (event.GetKey())
		{
		case Xen::KeyCode::KEY_Q:
			m_GizmoOperation = GizmoOperation::Translate;
			break;
		case Xen::KeyCode::KEY_W:
			m_GizmoOperation = m_EditorCamera->GetProjectionType() == Xen::CameraType::Orthographic ? GizmoOperation::Rotate2D : GizmoOperation::Rotate3D;
			break;
		case Xen::KeyCode::KEY_E:
			m_GizmoOperation = GizmoOperation::Scale;
			break;

		// Setting Up Key transformations here:
		case Xen::KeyCode::KEY_T:
			m_KeyTransformOperation = m_GameMode == GameMode::_2D ? KeyTransformOperation::Translate2D : KeyTransformOperation::Translate;
			break;
		case Xen::KeyCode::KEY_R:
			m_KeyTransformOperation = m_GameMode == GameMode::_2D ? KeyTransformOperation::Rotate2D : KeyTransformOperation::Rotate;
			break;
		case Xen::KeyCode::KEY_S:
			m_KeyTransformOperation = m_GameMode == GameMode::_2D ? KeyTransformOperation::Scale2D : KeyTransformOperation::Scale;
			break;

		case Xen::KeyCode::KEY_X:
			if (m_KeyTransformOperation & TranslateX || m_KeyTransformOperation & TranslateY || m_KeyTransformOperation & TranslateZ)
				m_KeyTransformOperation = TranslateX;
			else if (m_KeyTransformOperation & RotateX || m_KeyTransformOperation & RotateY || m_KeyTransformOperation & RotateZ)
				m_KeyTransformOperation = RotateX;
			else if (m_KeyTransformOperation & ScaleX || m_KeyTransformOperation & ScaleY || m_KeyTransformOperation & ScaleZ)
				m_KeyTransformOperation = ScaleX;
			break;
		case Xen::KeyCode::KEY_Y:
			if (m_KeyTransformOperation & TranslateX || m_KeyTransformOperation & TranslateY || m_KeyTransformOperation & TranslateZ)
				m_KeyTransformOperation = TranslateY;
			else if (m_KeyTransformOperation & RotateX || m_KeyTransformOperation & RotateY || m_KeyTransformOperation & RotateZ)
				m_KeyTransformOperation = RotateY;
			else if (m_KeyTransformOperation & ScaleX || m_KeyTransformOperation & ScaleY || m_KeyTransformOperation & ScaleZ)
				m_KeyTransformOperation = ScaleY;
			break;
		case Xen::KeyCode::KEY_Z:
			break;
		}
	}	
}